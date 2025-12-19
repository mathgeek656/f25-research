import sys

lim = 20
lower = 111
upper = 150
# 11919

def add(b, a):
	return (b[0]+a[0], b[1]+a[1])

def sub(b, a):
	return (b[0]-a[0], b[1]-a[1])

def neg(a):
	return (-a[0], -a[1])

def flip(a):
	return (a[0], -a[1])

def dot(a, b):
	return a[0]*b[0] + a[1]*b[1]

def cross(a, b):
	return a[1]*b[0] - b[1]*a[0]

def scale(v, s):
	return (v[0] * s, v[1] * s)

def basis(bas, a, b):
	return add(scale(bas[0], a), scale(bas[1], b))

def shorter(a, b):
	l1 = dot(a,a)
	l2 = dot(b,b)
	return l1 < l2

ret = {}

DEBUG = False

for vx in range(lim+1):
	for vy in range(lim+1):
		for wx in range(lim+1):
			for wy in range(-lim, lim+1):
				bas = [(vx, vy), (wx, wy)]
				if shorter(bas[1], bas[0]):
					bas = [bas[1], bas[0]]

				area = cross(bas[0], bas[1])
				if area == 0:
					continue

				## Find shortest equivalent vectors
				if DEBUG:
					print('Shortening {0}'.format(bas))
				while True:
					dv = sub(bas[1], bas[0])
					dv2 = add(bas[1], bas[0])
					if shorter(dv2, dv):
						dv = dv2

					if shorter(dv, bas[0]):
						bas = [dv, bas[0]]
					elif shorter(dv, bas[1]):
						bas = [bas[0], dv]
					else:
						break
					if DEBUG:
						print(' ===> {0}'.format(bas))

				## Canonicalize them
				if bas[0][0] < 0:
					bas[0] = neg(bas[0])
				if bas[1][0] < 0:
					bas[1] = neg(bas[1])
				if bas[0][1] < 0:
					bas = [flip(bas[0]), flip(bas[1])]

				area = cross(bas[0], bas[1])
				if area == 0:
					continue
				elif area < 0:
					bas = [bas[1], bas[0]]
					area = -area

				if bas[1][0] == 0 and bas[1][1] <= 0:
					continue

				if area not in ret:
					ret[area] = set()

				ret[area].add((bas[0],bas[1]))

def addRect(u, x, y, w, h):
	if w < 0:
		x = x - (-w - 1)
	if h < 0:
		y = y - (-h - 1)

	for yy in range(abs(h)):
		for xx in range(abs(w)):
			u.add((x + xx, y + yy))

def calcUnit(bas):
	my_ax = bas[0][0]
	my_ay = bas[0][1]
	my_bx = bas[1][0]
	my_by = bas[1][1]
	flip = False

	u = set()

	if my_ax < 0:
		my_ax = -my_ax
		my_ay = -my_ay
	if my_bx < 0:
		my_bx = -my_bx
		my_by = -my_by
	if my_ay < 0:
		flip = true
		my_ay = -my_ay
		my_by = -my_by

	if my_by <= 0:
		addRect(u, 0, 0, my_bx, my_ay)
		addRect(u, 0, my_ay, my_ax, -my_by)
	else:
		if my_by > my_ay:
			my_ax, my_ay, my_bx, my_by = my_bx, my_by, my_ax, my_ay

		# Now a is always higher up than b
		if my_ax <= my_bx:
			addRect(u, 0, 0, my_bx - my_ax, my_ay)
			addRect(u, 0, my_ay, my_ax, my_ay - my_by)
		else:
			# Ugh, nasty case. Jump to an equivalent case that's 
			# probably better behaved.
			return calcUnit(my_ax - my_bx, my_ay - my_by, my_bx, my_by)
	if flip:
		uf = set()
		for p in u:
			uf.add(flip(p))
		u = uf

	if len(u) == 0:
		sys.stderr.write(f'Empty unit {bas}\n')
	return u

def getNeighbours(p):
	## Get the six edge-connected neighbours of p
	return [add(p, v) for v in 
		[(1, 0), (0, 1), (-1, 1), (-1, 0), (0, -1), (1, -1)]]

def getCanon(bas, u, p):
	for dy in [-2, -1, 0, 1, 2]:
		for dx in [-2, -1, 0, 1, 2]:
			np = add(p, basis(bas, dx, dy))
			if np in u:
				return np

	sys.stderr.write(f'Something went wrong {bas} {u} {p}\n')
	return None

def getEquiv(bas, u):
	ret = {}

	## Get the halo of the translational unit: the empty cells adjacent 
	## to any cell in u.
	halo = set()
	edges = {}

	def getHaloEdges(h):
		ret = []
		for n in getNeighbours(h):
			if n in u:
				ret.append((n, h))
		return ret

	def hasFreeEdge(h):
		for e in getHaloEdges(h):
			if edges[e] == False:
				return True

		return False

	for p in u:
		for n in getNeighbours(p):
			if n not in u:
				halo.add(n)
				
				## Track all the edges between this halo cell and any 
				## cells in u.  Edge edge is represented by the coordinates
				## of the cells adjacent to it.
				for nn in getNeighbours(n):
					if nn in u:
						edges[(nn, n)] = False

	for k in halo:
		if hasFreeEdge(k):
			## This halo cell is still useful, because it constrains one
			## or more edges of u

			## This halo cell will generate a new equivalence constraint.
			## Figure out which u cell it's equivalent to, and keep that.
			canon = getCanon(bas, u, k)
			ret[k] = canon

			## Now mark off all edges constrained by this halo cell
			hes = getHaloEdges(k)
			dv = sub(canon, k)
			for he in hes:
				edges[he] = True
				## Also mark off edges relative to the translated copy
				## of k inside u.
				edges[(add(he[1], dv), add(he[0], dv))] = True

	return ret

tot = 0
tot2 = 0
print('#include "hexstruct.h"')
print("vector<hexStructure> hexes = {")

for ar in sorted(ret.keys()):
	#print('=== Area {0} === [{1}]'.format(ar, tot))
	for vw in ret[ar]:
		l = list(vw)
		u = calcUnit(vw)
		eq = getEquiv(vw, u)

		# print('BASIS: v = {0}, w = {1}'.format(l[0], l[1]))

		# for p in u:
		# 	print('    {0}'.format(p))
		# for k in eq:
		# 	print('    {0} = {1}'.format(k, eq[k]))
		if len(u) > upper:
			break
		if len(u) >= lower and len(u) <= upper: 
			print('hexStructure{', end="")
			print(len(u), end= ",")
			print(len(eq), end= ",\n")
			tot2 = tot2 + 1
			asdf = 1
			qwer = 1
			print('{')
			
			for p in u:
				#print('    {0}'.format(p))
				print('   make_pair({0}, {1})'.format(p[0], p[1]), end="")
				if (asdf < len(u)):
					asdf += 1
					print(",")
				else:
					print ()
			print('},')
			print('{')
			for k in eq:
				print('     make_pair(make_pair({0}, {1}), make_pair({2},{3}))'.format(k[0], k[1], eq[k][0], eq[k][1]), end="")
				if qwer < len(eq):
					qwer += 1
					print(",")
				else:
					print("")
				#print('    {0} = {1}'.format(k, eq[k]))
			print('}')
			print('},')

	tot = tot + len(ret[ar])
print("};")
#print(tot2)
