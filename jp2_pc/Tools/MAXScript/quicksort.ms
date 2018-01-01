fn QuickSort l r a = 
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t
				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t
				QuickSort l (i - 1) a
				QuickSort (i + 1) r a
			)
		return a
	)

fn QuickSort2 l r a b =
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t

					t2 = b[i]
					b[i] = b[j]
					b[j] = t2

				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t

				b[j] = b[i]
				b[i] = b[r]
				b[r] = t2

				QuickSort2 l (i - 1) a b
				QuickSort2 (i + 1) r a b
			)
		return #(a,b)
	)
