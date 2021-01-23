import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import xlrd

inpath = 'data.xls'  # excel文件所在路径
data = xlrd.open_workbook(inpath)
table = data.sheets()[0]  # 选定表
nrows = table.nrows#获取行号
table2 = []
table3 = []
table4 = []
table5 = []
befor = 0
after = 0
#x = np.linspace(1,500,int(nrows-1))
for i in range(1, nrows):#第0行为表头
        alldata = table.row_values(i)#循环输出excel表中每一行，即所有数据
        table5.append(alldata[0])
        table3.append(alldata[1])
        #table4.append(alldata[2])
"""        table2.append(alldata[0])
        
        
        

plt.plot(x,table2,color="darkblue",linewidth=0.1)
plt.plot(x,table3,color="red",linewidth=0.1)
plt.plot(x,table4,color="black",linewidth=0.1)
"""
tebshow = []
intep = 1

y = np.linspace(1,100,len(table5))
plt.plot(y,table5,color="black",linewidth=0.1)
plt.plot(y,table3,color="red",linewidth=0.1)
#plt.scatter(y,table3,color="red",linewidths=0.1)
print(table5)
plt.savefig("plot.jpg",dpi = 1000)
plt.show()
