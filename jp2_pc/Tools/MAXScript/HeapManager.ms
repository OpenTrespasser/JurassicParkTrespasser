fn round_to val n = 
(
	local mult = 10.0 ^ n
	(floor ((val * mult) + 0.5)) / mult
)

Utility HeapManager "Heap Manager"
(

group "Heap Ops"
(
button UpdateHeapInfo "Get Heap Info" width:140
spinner heapincrease "MB" range:[1,64,1] type:#integer fieldwidth:30 offset:[-80,0]
button addheap "Add to Heap" offset:[30,-24]
button garbage "Collect Garbage" width:140
)
group "Heap Info"
(
label heapinfo1 ">---Current HeapSize---<"
label heapinfo2 ""
label heapinfo3 ">---Current Heap Free---<"
label heapinfo4 ""
)

on HeapManager open do
(
heapsizevalue = round_to ((heapsize as float/ 1024) / 1024) 2
heapfreevalue = round_to ((heapfree as float/ 1024) / 1024) 2
heapInfo2.text = (heapsizevalue as string + " MB")
heapInfo4.text = (heapfreevalue as string + " MB")
)

on garbage pressed do 
(
gc()
heapsizevalue = round_to ((heapsize as float/ 1024) / 1024) 2
heapfreevalue = round_to ((heapfree as float/ 1024) / 1024) 2
heapInfo2.text = (heapsizevalue as string + " MB")
heapInfo4.text = (heapfreevalue as string + " MB")
)

on Addheap pressed do
(
heapsize += (heapincrease.value * 1000000)
heapsizevalue = round_to ((heapsize as float/ 1024) / 1024) 2
heapfreevalue = round_to ((heapfree as float/ 1024) / 1024) 2
heapInfo2.text = (heapsizevalue as string + " MB")
heapInfo4.text = (heapfreevalue as string + " MB")
)

on UpdateHeapInfo pressed do
(
heapsizevalue = round_to ((heapsize as float/ 1024) / 1024) 2
heapfreevalue = round_to ((heapfree as float/ 1024) / 1024) 2
heapInfo2.text = (heapsizevalue as string + " MB")
heapInfo4.text = (heapfreevalue as string + " MB")
)

)