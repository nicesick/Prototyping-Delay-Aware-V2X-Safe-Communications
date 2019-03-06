import matplotlib.pyplot as plt
import pandas as pd
from pandas import ExcelFile
from pandas import ExcelWriter

file1 = r'resources/data_visualization/data/results_CS1&H49.xlsx'
#file2 = r'resources/data_visualization/data/results_voluntary.xlsx'

df_1 = pd.read_excel(file1, sheet_name='condition1')
# df_2 = pd.read_excel(file1, sheet_name='condition4')
# df_3 = pd.read_excel(file2, sheet_name='condition2')
# df_4 = pd.read_excel(file2, sheet_name='condition4')

condition1 = df_1.select_dtypes(include=['float64'])
# condition2 = df_1.select_dtypes(include=['float64'])
# condition3 = df_1.select_dtypes(include=['float64'])
# condition4 = df_1.select_dtypes(include=['float64'])

condition1.plot()
plt.xlabel("Iteration")
plt.ylabel("Network Latency")
# condition2.plot()
# plt.xlabel("Iteration")
# plt.ylabel("Network Latency")
# condition3.plot()
# plt.xlabel("Iteration")
# plt.ylabel("Network Latency")
# condition4.plot()
# plt.xlabel("Iteration")
# plt.ylabel("Network Latency")
plt.show()
