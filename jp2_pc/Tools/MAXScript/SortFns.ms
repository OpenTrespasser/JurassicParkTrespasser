fn SortUp foo a =
	(
		for i = 1 to a.count do
			(
				min = i
				for j = 1 to a.count do
					if a[j] > a[min] do
						(
							min = j
							t = a[min]
							a[min] = a[i]
							a[i] = t
						)
			)
		return a
	)

fn SortDn foo a =
	(
		for i = 1 to a.count do
			(
				min = i
				for j = 1 to a.count do
					if a[j] < a[min] do
						(
							min = j
							t = a[min]
							a[min] = a[i]
							a[i] = t
						)
			)
		return a
	)

fn SortUp2 foo a b =
	(
		for i = 1 to a.count do
			(
				min = i
				for j = 1 to a.count do
					if a[j] > a[min] do
						(
							min = j
							t = a[min]
							a[min] = a[i]
							a[i] = t

							t = b[min]
							b[min] = b[i]
							b[i] = t
						)
			)
		return #(a,b)
	)
