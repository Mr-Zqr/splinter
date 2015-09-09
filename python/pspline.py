# This file is part of the SPLINTER library.
# Copyright (C) 2012 Bjarne Grimstad (bjarne.grimstad@gmail.com).
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


import sys
from approximant import Approximant
from utilities import *


class PSpline(Approximant):
	def __init__(self, dataTableOrFileName, smoothing = None):
		# If string we load the PSpline from the file
		if isString(dataTableOrFileName):
			fileName = getCString(dataTableOrFileName)
			self._handle = SPLINTER.call(SPLINTER.getHandle().pspline_load_init, fileName)
		
		# Else, construct PSpline from DataTable
		else:
			dataTable = dataTableOrFileName
			if smoothing is None:
				smoothing = 0.03
			self._handle = SPLINTER.call(SPLINTER.getHandle().pspline_init, dataTable.getHandle(), smoothing)
		
		
if __name__ == "__main__":
	import SPLINTER
	SPLINTER.load("/home/anders/SPLINTER/build/release/libsplinter-matlab-1-4.so")
	
	
	from datatable import DataTable
	
	def f(x):
		return x[0]*x[1]
	
	d = DataTable()
	for i in range(10):
		for j in range(10):
			d.addSample([i,j], f([i,j]))
	
	b = PSpline(d, 0.03)
	for i in range(10):
		for j in range(10):
			print(str(b.eval([0.9*i,0.9*j])) + " == " + str(0.81*i*j))
	
	print(b.evalJacobian([3,3]))
	print(b.evalHessian([3,3]))
	
	b.save("test.pspline")
	b2 = PSpline("test.pspline")
	
	print(b.eval([2,3]))
	print(b2.eval([2,3]))