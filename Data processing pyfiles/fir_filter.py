#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 24 18:42:36 2020

@author: tomek
"""



    
    
    
    
import numpy as np


#CLASS SECTION
class FIR_filter:
    def __init__(self,_coefficients):
        self.buffer = np.zeros(len(_coefficients))
        self.h = np.array(_coefficients)
        self.counter = 0
        self.y_n = np.zeros(len(_coefficients))
        
        
    def dofilter(self,value):
        #print(self.counter)
        #print(type(self.counter))
        #print(value)
        #print(type(value))
        self.buffer[self.counter] = value
        
        i = 0
        while i <= self.counter:
            self.y_n[i] = self.buffer[self.counter - i] * self.h[i]
            i = i + 1
            
            
        i = self.counter + 1
        while i < len(self.buffer):
            self.y_n[i] = self.buffer[len(self.buffer) + self.counter - i] * self.h[i]
            i = i + 1
            
            
        self.sum = sum(self.y_n)
        self.counter = self.counter + 1
        if self.counter == len(self.buffer):
            self.counter = 0
        
        return self.sum
    
    

    
                #MAIN  

    
if __name__ == '__main__':
    
    test_data = np.array([0,1,0,1,1,1])
    test_h = np.array([1,2,3,4])
    
    x_test = FIR_filter(test_h)
    test_result = []
    counter_list = []
    
    x_test.counter = 0
    for i in range(len(test_data)):
        sample=test_data[i]
        test_result.append(x_test.dofilter(sample))
        counter_list.append(x_test.counter)
    
    print("counter test:")
    A = np.array([1,2,3,0,1,2])
    if np.array_equal(counter_list, A) == True:
        print("yes")
    else:
        print("no")
        
    print("buffer test:")
    A = np.array([1.,1.,0.,1.])
    if np.array_equal(x_test.buffer, A) == True:
        print("yes")
    else:
        print("no")
        
        
    print("output test:")
    A = np.array([1.,2.,3.,0.])
    if np.array_equal(x_test.y_n, A) == True:
        print("yes")
    else:
        print("no")
        
    test_data = np.array([1,0,0,0])
    test_h = np.array([1,2,3,4])
    
    x_test = FIR_filter(test_h)
    test_result = []
    
    x_test.counter = 0
    for i in range(len(test_data)):
        sample=test_data[i]
        test_result.append(x_test.dofilter(sample))
    
    print("unit step test:")
    if np.array_equal(test_result, test_h) == True:
        print("yes")
    else:
        print("no")
        
