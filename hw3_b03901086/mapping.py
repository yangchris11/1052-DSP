#!/usr/bin/env python
# -*- coding: utf-8 -*- 

ZY = {}
empty_dict = {} 

with open('Big5-ZhuYin.map', 'rb') as fin :
    for line in fin:

        line = line[:-1]

        data = line.split(' ',1)
        CHI_char = data[0]   # the Chinese character

        ZhuYin_list = data[1].split('/')

        for ZhuYin in ZhuYin_list :
            if ZhuYin[:2] not in empty_dict :
                empty_dict[ZhuYin[:2]] = [CHI_char]
            else :
                if CHI_char not in empty_dict[ZhuYin[:2]] :
                    empty_dict[ZhuYin[:2]].append(CHI_char) 

for key in empty_dict :
    print key ,
    for value in empty_dict[key] :
        print value ,
    print ''
    for value in empty_dict[key] :
        print value,
        print value
    

for each_key in ZY:
    ZY[each_key] = list(set(ZY[each_key]))



