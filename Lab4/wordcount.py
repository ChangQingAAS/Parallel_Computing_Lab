#!/usr/bin/env python
#-*-conding:utf-8-*-
 
import logging
from operator import add
from pyspark import SparkContext
 
logging.basicConfig(format='%(message)s', level=logging.INFO)  
 
#import local file
input_file_name = "file:///var/lib/hadoop-hdfs/spark_test/input.txt"  
# out_file_name = "file:///var/lib/hadoop-hdfs/spark_test/output"
 
sc = SparkContext("local","wordcount app")
 
# input_file rdd object
text_file = sc.textFile(input_file_name)
 
# counts
counts = text_file.flatMap(lambda line: line.split(" ")).map(lambda word: (word, 1)).reduceByKey(lambda a, b: a + b)
output = counts.collect()
for (word, count) in output:
print("%s %i" % (word, count))