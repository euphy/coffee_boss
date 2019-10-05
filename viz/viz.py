from datetime import datetime

from matplotlib.axes import Axes
from matplotlib.figure import Figure
import numpy as np
from pandas import read_csv
import pandas as pd
from matplotlib import pyplot as plt
import matplotlib.ticker as ticker

column_names = ['datestamp', 'date', 'time', 'weight']
df = read_csv('../output/datr20190923.csv', names=column_names, parse_dates=True, infer_datetime_format=True)
df['datetime'] = pd.to_datetime(df['datestamp'])
df.set_index('datetime')
del df['datestamp']
del df['time']
del df['date']

small_window = {'name': 'rolling4', 'size': 4}
large_window = {'name': 'rolling36', 'size': 36}
events = 'events'

df[small_window['name']] = df['weight'].rolling(small_window['size']).median()
df[large_window['name']] = df['weight'].rolling(large_window['size']).median()

# produce a series, based on the results of the large_window median filter,
# that looks for large changes that might be interesting.

df['pct_change'] = df[large_window['name']].pct_change()


print(df)
print(df.info())

# Type hinting. Not sure if this is kosher or not.
fig1: Figure
ax: Axes
fig1, ax = plt.subplots()
df.plot(x='datetime', y=[large_window['name'], small_window['name']], ax=ax)
# fig2 = ax.twinx()
# df.plot(x='datetime', y='pct_change', ax=ax)


#  pretty printing
# count = df['datetime'].count()
# no_of_ticks = 24
# size_of_segment = int(count / no_of_ticks)
#
# # two lists, one is a list of indices regularly spread out across the df
# indices = list()
# tick_labels = list()
# for i in range(0, no_of_ticks):
#     position = (size_of_segment * i) + 42/2
#     indices.append(position)
#     tick_labels.append(df['datetime'][position])
#
ax.grid(True, which='major')
# ax.set_xticks(indices)
# ax.set_xticklabels(labels=tick_labels, rotation='vertical')

plt.tight_layout()
plt.show()
