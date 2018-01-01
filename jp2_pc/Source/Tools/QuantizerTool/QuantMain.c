#include <stdio.h>
#include <stdlib.h>
#include "neuquant.h"
#include "\lib\h\hybrid.h"
#include <math.h>

int DITHER = 5;

char infile[256];
char outfile[256];

char fname[256];
char palette[768];
int  samplefac = 10;
int  inwidth=0,inheight=0;
int  outwidth=0,outheight=0;
int  view = 0;
int  writefile = 1;
char mypal[768];
char *buffer;
int  vesa;
int  vesabuf = 0;
int  orig8bit = 0;

char *irqstack;

void show_smaller(char *s, int width, int height, int ww, int wh)
{
    int px,py,xs,ys,i;

    px = width;
    py = height;

    xs = (ww-px)/2;
    ys = (wh-py)/2;

    for (i=0; i<py; i++)
        MEM_copy (buffer+(i+ys)*ww+xs,s+i*width,px);

    while (!KBD_buffer[KEY_SPACE] && !KBD_buffer[KEY_ESC]) {};
}

void set_vesa_screen (int start);
#pragma aux set_vesa_screen = \
" pushad " \
" pushfd " \
" cli " \
" push edx " \
" mov edx,03DAh " \
"W: in al,dx " \
" test al,8 " \
" jnz W " \
" pop edx " \
" mov eax,04F07h " \
" xor ebx,ebx " \
" xor ecx,ecx " \
" int 10h " \
" popfd"\
" popad"\
parm [edx];

//-----------------------------------------------------------------------------

void switch_page()
{
    set_vesa_screen (vesabuf);
    vesabuf ^= 480;
    buffer = (char *)linAddr+(vesabuf*640);
}

void show_larger(char *s, int width, int height, int ww, int wh)
{
    int x,y,i,px,py;

    x = (width-ww)/2;
    y = (height-wh)/2;

    while (!KBD_buffer[KEY_SPACE] && !KBD_buffer[KEY_ESC])
    {
        if (x>width-ww) x= width-ww;
        if (x<0) x = 0;
        if (y>height-wh) y = height-wh;
        if (y<0) y = 0;

        px = (width-x);  if (px>ww) px = ww;
        py = (height-y); if (py>wh) py = wh;
        if (px>width) px = width;
        if (py>height) py = height;

        for (i=0; i<py; i++)
            MEM_copy (buffer+i*ww,s+(i+y)*width+x,px);

        if (KBD_buffer[KEY_LEFT]) x--;
        if (KBD_buffer[KEY_RIGHT]) x++;
        if (KBD_buffer[KEY_UP]) y--;
        if (KBD_buffer[KEY_DOWN]) y++;

        if (vesa)
        {
            switch_page();
            VGA_wait_vri();
        }
    }
}

void view_image (char *s, char *p, int width, int height)
{
    int     VBE;
    int     ww,wh;
    int     i;
    int     px,py;
    char    VBEstr[5];
    int     xs,ys;

    vesa = 0;
    VBE  = VESA_detect();

    sprintf(VBEstr,"%x",VBE);
    VBE = atoi(VBEstr);

    if (VBE>=200)
        vesa = 1;

    for (i=0; i<768; i++)
        mypal[i]=p[i]/4;

    if (vesa)
    {
        if (VESA_set_mode(640,480,8))
            vesa = 0;
        else
        {
            ww = 640;
            wh = 480;
            buffer = (char *)linAddr;
        }
    }

    if (!vesa)
    {
        VGA_set_mode (0x13);
        ww=320;
        wh=200;
        buffer = (char*)VGA_screen;
    }


    VGA_set_palette(mypal);

    if (width>ww || height>wh)
        show_larger  (s,width,height,ww,wh); else
        show_smaller (s,width,height,ww,wh);

    VGA_set_mode(0x3);

}

int save_tga (char *tgafile, char *buffer, char *palette, int width, int height)
{
    FILE    *of;
    TGA     TGA_dest;
    char    tgapal[768];
    int     i;

    of = fopen(tgafile,"wb");

    if (!of)
        return -1;

    TGA_dest.idsize = 0;        // no id
    TGA_dest.cmap_type = 1;     // color map included
    TGA_dest.imagetype = 1;     // uncompressed, truecolor

    TGA_dest.cmap_first = 0;    // no first color
    TGA_dest.cmap_size = 256;   // no palette
    TGA_dest.cmap_bits = 24;    // 24 bits per color

    TGA_dest.xorigin = 0;       // image origin
    TGA_dest.yorigin = 0;

    TGA_dest.width  = width;    // image size
    TGA_dest.height = height;

    TGA_dest.bitdepth = 8;      // byte per pixel
    TGA_dest.descriptor = 0x20; // start top left

    for(i=0;i<256;i++)
    {
        tgapal[i*3+0] = palette[i*3+2];
        tgapal[i*3+1] = palette[i*3+1];
        tgapal[i*3+2] = palette[i*3+0];
    }

    fwrite(&TGA_dest, 1, sizeof(TGA), of);
    fwrite (tgapal,1,768,of);
    fwrite (buffer,1,width*height,of);

    fclose (of);
    return 0;

}

void show_info()
{
    printf ("The program quantizes hi- and truecolour images to optimal 8-bit palette.\n");
    printf ("The algorithm uses Kohonen neural networks for creating the palette.\n");
    printf ("Error-correction dithering is applied in the quantization phase.\n");
    printf ("The program can scale the output image to any size. If the size specified");
    printf ("is larger than the input image, bilinear filtering is used. If the output\n");
    printf ("image is smaller, area sampling is applied.\n");
    printf ("\n");
    printf ("The image quality and speed of the learning phase can be tuned with\n");
    printf ("the -s parameter. The amount of random diffuse dithering can be tuned\n");
    printf ("with the -d parameter. When quantizing images which use a long 'range'\n");
    printf ("of colours (such as a smooth transition from black to white with 64\n");
    printf ("grays), the random dithering can improve image quality considerably.\n");
    printf ("\n");
    printf ("The program takes both compressed and uncompressed targa files as input\n");
    printf ("but currently only outputs uncompressed ones. If the view (-v) option is\n");
    printf ("selected, the quantized image is displayed on the screen. If UniVBE is loaded\n");
    printf ("into memory or VGA card supports VESA 2.0, the screen mode used is 640x480,\n");
    printf ("otherwise it is 320x200. If output image size is larger than the display size,\n");
    printf ("use the cursor keys to pan the image. If input image is 8-bit and no scaling is\n");
    printf ("requested and the -v paramete is used, whole quantization process is skipped\n");
    printf ("and picture is just displayed.\n");
    printf ("\n");
    printf ("You can exit the program at any time by pressing <ESC>.\n");
}

void myerror2()
{
    KBD_free();
    MEM_shutdown();
    exit(1);
}

void myerror()
{
    printf ("\n");
    printf ("TGA image quantization program using Kohonen neural network\n");
    printf ("author wili@hybrid.org 1996, based on code by anthony dekker\n\n");

    printf ("Syntax: %s <infile24.tga> [<outfile8.tga>] [options]\n",fname);
    printf ("Options can be the following:\n\n");

    printf ("-h         for more information about the program\n");
    printf ("-v         view 8-bit image\n");
    printf ("-x<value>  output image width (default is same as input)\n");
    printf ("-y<value>  output image height\n");
    printf ("-s<value>  sample factor (1=best/slowest,10=default,1000=fastest)\n");
    printf ("-d<value>  random dither value (1=none,7=default)\n");
    printf ("\nfor example: %s truecol.tga 8bit.tga -x320 -y200 -s5 -d10\n",fname);

    myerror2();
}


void check_abort()
{
    if (!KBD_buffer[KEY_ESC]) return;
    printf ("\n<ESC> detected. Aborting.\n");
    myerror2();
}
void load_8bit (char *dest, TGA *tga)
{
    char *tmp,*p;
    char pal[768];
    int i,s;


    s = tga->width*tga->height;

    tmp     = MEM_allocate(s);
    if (!tmp)
    {
        printf ("ERROR: out of memory\n");
        myerror2();
    }

    TGA_rle_uncompress (tmp,tga,1);
    TGA_get_palette(tga,pal);

    for (i=0; i<s; i++)
    {
        p = pal+((int)tmp[i])*3;

        dest[i*3] = p[2];
        dest[i*3+1] = p[1];
        dest[i*3+2] = p[0];
    }

    MEM_free(tmp);
}

int clamp (int a)
{
    if (a<0) return 0; else
    if (a>255) return 255; else
    return a;
}

int errorr,errorg,errorb;

void quantize_pixel (char *dest, char *source)
{
    int r,g,b;
    char *p;

    r = clamp((int)source[0]-errorr);
    g = clamp((int)source[1]-errorg);
    b = clamp((int)source[2]-errorb);

    *dest = inxsearch (r,g,b);

    p = palette+*dest*3;


    errorr=((int)p[0]-source[0])*0.25+rand()%DITHER-DITHER/2;
    errorg=((int)p[1]-source[1])*0.25+rand()%DITHER-DITHER/2;
    errorb=((int)p[2]-source[2])*0.25+rand()%DITHER-DITHER/2;

}

char *load_picture (char *name, int *inwidth, int *inheight, int *inlen8, int *inlen)
{
    char *pic;
    TGA  *tga;
    int  i,t;
    char pallie[768];

    tga = FILE_allocate(name);
    if (!tga)
    {
        printf ("ERROR: Cannot open file <%s>\n",name);
        myerror2();
    }

    if (tga->bitdepth!=24 && tga->bitdepth!=32 && tga->bitdepth!=8)
    {
        printf("ERROR: This program operates only on 8/24/32 bit TGA images\n");
        myerror();
    }

    *inwidth  = tga->width;
    *inheight = tga->height;
    *inlen8   = (*inwidth)*(*inheight);
    *inlen    = 3*(*inlen8);

    pic = MEM_allocate(*inlen);

    if (!pic)
    {
        printf ("ERROR: Not enough memory to open file\n");
        myerror2();
    }

    printf ("loaded input image  <%s> (%d x %d %d bits)\n",name,*inwidth,*inheight,tga->bitdepth);

    if (tga->bitdepth==8)
    {
        if (view && !outwidth && !outheight)
        {
            TGA_rle_uncompress (pic,tga,1);
            TGA_get_palette (tga,pallie);

            view_image(pic,pallie,*inwidth,*inheight);
            myerror2();
        } else
            load_8bit (pic,tga);
    }
    else
        TGA_rle_uncompress (pic,tga,3);

    // swap picture order

    for (i=0; i<*inlen8; i++)
    {
        t = pic[i*3];
        pic[i*3] = pic[i*3+2];
        pic[i*3+2] = t;
    }

    MEM_free(tga);

    return pic;
}

void quantize_image (char *dest, char *source, int pixels)
{
    char *p;
    int  percadd;
    int  perc,i;

    p       = source;
    percadd = (pixels/70);
    perc    = percadd;

    for (i=0; i<pixels; i++,p+=3)
    {
        if (i>perc)
        {
            check_abort();
            printf("%cQuantizing image (%d%%)",13,(i*100)/pixels);
            perc = i+percadd;
            fflush(stdout);
        }

        quantize_pixel (dest+i,p);
    }
}

int FTOI(float foo);
#pragma aux FTOI = \
"push eax"\
"fistp dword ptr [esp]" \
"pop eax" \
parm [8087] value [eax];

int scalex;
int scaley;

// bilinear interpolation

int get_point (char *src, int ix, int iy, int channel)
{
    if (ix>=scalex || ix<0) return 0;
    if (iy>=scaley || iy<0) return 0;

    return src[(ix+iy*scalex)*3+channel];

}

float get_pixel (char *src, int channel, float x, float y)
{
    float fx,fy,f0,f1;
    int ix,iy;
    char *s;
    int p[4];

    x*=scalex;                    // scale to input image size
    y*=scaley;

    if (x<0) x = 0;
    if (y<0) y = 0;

    fx = 1.0-(x-floor(x));        // get fractional part for x
    fy = 1.0-(y-floor(y));        // get fractional part for y

    ix = x;                       // convert to integer
    iy = y;

    s = src+(ix+iy*scalex)*3+channel;

    p[0] = get_point (src,ix,iy,channel);
    p[1] = get_point (src,ix+1,iy,channel);
    p[2] = get_point (src,ix,iy+1,channel);
    p[3] = get_point (src,ix+1,iy+1,channel);

    f0 = p[0]*fy+p[2]*(1.0-fy);
    f1 = p[1]*fy+p[3]*(1.0-fy);

    return (fx*f0 + (1.0-fx)*f1);           // bi-linear interpolation
}

char *scale_image (char *source, int inwidth, int inheight, int outwidth, int outheight)
{
    char *d,*d2;
    int   l;
    int   x,y,i;
    float fx,fy,fxadd,fyadd;
    float sx,sy,s2,gx,gy,v;

    l = outwidth*outheight*3;
    d2 = d = MEM_allocate(l);       // just to make sure..

    if (!d)
    {
        printf ("ERROR: Not enough memory to scale image\n");
        myerror2();
    }

    scalex  = inwidth;
    scaley  = inheight;

    sx = (float)(inwidth-1)/outwidth/2.0;
    sy = (float)(inheight-1)/outheight/2.0;


    s2 = 0;

    for (gy = -sy; gy <= sy; gy+=1.0)
    for (gx = -sx; gx <= sx; gx+=1.0)
        s2+=1.0;

    s2 = 1.0/s2;

    if (inwidth==outwidth && inheight==outheight)   // 1:1 aspect ratio scale
    {
        MEM_copy (d2,source,l);
        return d2;
    }

    fxadd = 1.0/(outwidth-1);
    fyadd = 1.0/(outheight-1);

    for (fy = 0, y=0; y<outheight; y++, fy+=fyadd)
    {
        printf ("%cScaling and filtering image (%d%%)",13,(100*y)/outheight);
        check_abort();
        for (fx = 0, x=0; x<outwidth; x++, fx+=fxadd, d2+=3)
        for (i=0; i<3; i++)
        {
            v = 0;

            for (gy = -sy; gy <= sy; gy+=1.0)
            for (gx = -sx; gx <= sx; gx+=1.0)
                v+= get_pixel (source,i,fx+gx/scalex,fy+gy/scaley);

            d2[i] = FTOI(v*s2-0.5);
        }

        fflush (stdout);
    }
    return d;
}

void parse_command_line (int argc, char *argv[])
{
    int fc=0,i;

    for(i=1;i<argc;i++)
	{
        strlwr (argv[i]);

        if(!strncmp(argv[i],"-h",2))
        {
            show_info();
            myerror2();
        }

        if(!strncmp(argv[i],"-v",2))
        {
            view = 1;
            continue;
        }

        if(!strncmp(argv[i],"-s",2))
        {
            samplefac = atoi(argv[i]+2);
            if (samplefac<0)
                samplefac = 1;
            else
            if (samplefac>1000)
                samplefac = 1000;

            continue;
        }

        if(!strncmp(argv[i],"-x",2))
        {
            outwidth = atoi(argv[i]+2);
            if (outwidth<=0)
                outwidth = 1; else
            if (outwidth>4096)
                outwidth = 4096;

            continue;
        }

        if(!strncmp(argv[i],"-y",2))
        {

            outheight = atoi(argv[i]+2);
            if (outheight<=0)
                outheight = 1; else
            if (outheight>4096)
                outheight = 4096;

            continue;
        }

        if(!strncmp(argv[i],"-d",2))
        {

            DITHER = atoi(argv[i]+2);
            if (DITHER<=0)
                DITHER = 1; else
            if (DITHER>255)
                DITHER=255;

            continue;
        }

        if (!strncmp(argv[i],"-",1))
        {
             printf ("ERROR: unknown option <%s>\n",argv[i]);
             myerror();
        }
           
        if (fc==0)
        {
            strcpy (infile,argv[i]);
            fc++;
            continue;
        }
        else
        if (fc==1)
        {
            strcpy (outfile,argv[i]);
            fc++;
            continue;
        }
        else
        {
            printf ("ERROR: unknown option <%s>\n",argv[i]);
            myerror();
            continue;
        }
    }

    if (fc==0)
    {
        printf ("ERROR: input file not specified\n");
        myerror();
    } else
    if (fc==1)
        writefile = 0;

}

void clear_line()
{
    printf ("%c                                                      ",13);
}

//-----------------------------------------------------------------------------


void main(int argc, char *argv[])
{
    TGA  *tga;
    char *pic,*p,*finalpic;
    char *outpic;

    int  inlen,outlen,inlen8,outlen8;

    MEM_init(100);
    strncpy (fname,argv[0],79);     // name of executable

    irqstack = MEM_allocate(16384);
    IRQ_init(irqstack);
    KBD_hook();

    MEM_clear_allocs = 0;           // no need to clear on allocate

    parse_command_line (argc, argv);

    if (!writefile && !view)
    {
        printf ("No output file specified and -v flag not set. Quitting.\n");
        exit(1);
    }
    strlwr(infile);

    if (writefile)
    {
        strlwr(outfile);
        if (strcmp(outfile+strlen(outfile)-4,".tga"))
            strcat(outfile,".tga");
    }

    if (strcmp(infile+strlen(infile)-4,".tga"))
        strcat(infile,".tga");

    pic = load_picture (infile, &inwidth, &inheight, &inlen8, &inlen);

    if (!outwidth)
        outwidth  = inwidth;        // use source file size as default

    if (!outheight)
        outheight = inheight;

    finalpic  = scale_image (pic,inwidth,inheight,outwidth,outheight);

    if (!finalpic)
    {
        printf ("ERROR: Not enough memory for temporary scale image\n");
        myerror2();
    }


    MEM_free (pic);

    outlen8   = outwidth*outheight;
    outlen    = outlen8*3;

    clear_line();

    outpic    = MEM_allocate (outlen8);

    if (!outpic)
    {
        printf ("ERROR: Not enough memory for 8-bit image\n");
        myerror2();
    }

    initnet        (finalpic,outlen,samplefac);
    learn          ();
    unbiasnet      ();
    copy_colourmap (palette);
    inxbuild       ();
    quantize_image (outpic, finalpic, outlen8);
    if (writefile)
    {
        printf ("%cSaving output image <%s> (%d x %d 8 bits) (%d kB)\n",13,outfile,outwidth, outheight, outlen8/1024);

        if (save_tga (outfile, outpic, palette, outwidth, outheight)==-1)
        {
            printf ("ERROR: cannot write output file\n");
            myerror2();
        }
    } else
    printf ("%cNo output file was specified. Saving skipped.\n",13);

    if (view)
        view_image(outpic,palette,outwidth,outheight);

    myerror2();
}
