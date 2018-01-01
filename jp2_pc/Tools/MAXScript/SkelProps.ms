fn KeyName CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return CName
		else
			return (substring CName 1 (DashPos - 1))
	)

Utility SkelProps "Skeleton Props"
	(
		local TrigDefaults = "\r\r\ni FireCount = -1;\r\r\ni AlphaChannel = 0;\r\r\nb Wrap = false;\r\r\nb Merge = false;\r\r\nb Split = false;\r\r\n"
		local EAXAudioEnter = ("s Class = \"CLocationTrigger\";\r\r\nb PlayerEnterTrigger = true;\r\r\ni ActionType = 29;\r\r\ni AudioEnvironment = xx;\r\r\nf Reverb = x.x;\r\r\nf ReverbDecay = x.x;\r\r\nf ReverbDamping = x.x;\r\r\ni BoundVol = 1;\r\r\n" + TrigDefaults)
		local EAXAudioLeave = ("s Class = \"CLocationTrigger\";\r\r\nb PlayerLeaveTrigger = true;\r\r\ni ActionType = 29;\r\r\ni AudioEnvironment = xx;\r\r\nf Reverb = x.x;\r\r\nf ReverbDecay = x.x;\r\r\nf ReverbDamping = x.x;\r\r\ni BoundVol = 1;\r\r\n" + TrigDefaults)

		group "Skeleton Props Options"
			(
				listBox TriggerProps "Trigger Props" items:#("Aud: EAX Enter Defaults", "Aud: EAX Leave Defaults") height:5
				button Apply "Apply" width:140
			)
		
		group "EAX Box Trigger Creation"
			(
				Button BoxFromBBox "Create EAX Triggers" width:140
			)

		on BoxFromBBox pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				for i = 1 to oc do
					(
						obj = ObjArray[i]
						oRot = obj.rotation
						oPos = obj.pos
						oOffset = obj.pivot - obj.center
						obj.rotation = (quat 0 0 0 1)
						obj.pos = oPos
						ht = ((obj.max).z - (obj.min).z)
						wth = ((obj.max).x - (obj.min).x)
						len = ((obj.max).y - (obj.min).y)
						b = box height:ht width:wth length:len
						b.pivot = b.center
						b.pos = obj.center
						b.pivot = obj.pivot
						b.rotation = oRot
						b.pos = oPos
						b.name = obj.name
						obj.rotation = oRot
						obj.pos = oPos
						if TriggerProps.selection == 1 or TriggerProps.selection == 2 do
							(
								c = copy b
								b.wirecolor = (color 41 232 255)
								c.wirecolor = (color 29 108 181)
								b.width -= .5; b.length -= .5; b.height -= .5;
								b.name = "Trig_EAX_" + (keyname obj.name) + "_Enter"
								c.name = "Trig_EAX_" + (keyname obj.name) + "_Leave"
								setUserPropBuffer b EAXAudioEnter
								setUserPropBuffer c EAXAudioLeave
							)
					)
			)

		on Apply pressed do
			for o in selection do
				(
					if TriggerProps.selection == 1 then setUserPropBuffer o EAXAudioEnter
					if TriggerProps.selection == 2 then setUserPropBuffer o EAXAudioLeave
				)

	)