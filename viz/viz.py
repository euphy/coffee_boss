from datetime import datetime

from matplotlib.axes import Axes
from matplotlib.figure import Figure
import numpy as np
from pandas import read_csv
import pandas as pd
from matplotlib import pyplot as plt
import matplotlib.ticker as ticker

column_names = ['datestamp', 'date', 'time', 'weight']
df = read_csv('../output/datr20190911.csv', names=column_names, parse_dates=True, infer_datetime_format=True)
df = df.append(read_csv('../output/datr20190912.csv', names=column_names, parse_dates=True, infer_datetime_format=True))
# df = df.append(read_csv('../output/datr20190913.csv', names=column_names, parse_dates=True, infer_datetime_format=True))
# df = df.append(read_csv('../output/datr20190914.csv', names=column_names, parse_dates=True, infer_datetime_format=True))

df.reset_index(drop=True, inplace=True)
print(df)
df['datetime'] = pd.to_datetime(df['datestamp'])
df['index'] = df['datetime']
df.set_index('index', inplace=True)
del df['datestamp']
del df['time']
del df['date']

small_window = {'name': 'rolling4', 'size': 4}
large_window = {'name': 'rolling36', 'size': 36}
events = 'events'

df[small_window['name']] = df['weight'].rolling(small_window['size']).median()
df[large_window['name']] = df['weight'].rolling(large_window['size']).median()

print(df)
# produce a series, based on the results of the large_window median filter,

df['pct_change'] = df[small_window['name']].pct_change()
df['diff'] = df[small_window['name']].diff(periods=8)
df['diff'] = df['diff'].shift(-8)

threshold = 300.0
df['thresholded'] = (df['diff'] > threshold)
df['highlight'] = (df['thresholded'] == True) & (df['thresholded'].shift(1) == False)

print(df.info())
print(df)

# this isolates the points where the weight increases, but not when it drops back down again.
highlights = df[df['highlight']][['datetime', 'weight']]

print(highlights)

# Type hinting. Not sure if this is kosher or not.
fig1: Figure
fig1, (ax1, ax2, ax3) = plt.subplots(nrows=3, ncols=1, sharex='all')
fig1.set_size_inches(8, 9)

ax1.grid(b=True, which='major', color='#666666', linestyle='-')
ax1.minorticks_on()
ax1.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax2.grid(b=True, which='major', color='#666666', linestyle='-')
ax2.minorticks_on()
ax2.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

ax3.grid(b=True, which='major', color='#666666', linestyle='-')
ax3.minorticks_on()
ax3.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

df.plot(y=[small_window['name'], 'thresholded'], secondary_y=['thresholded'], ax=ax1)
df.plot(y=[small_window['name']], ax=ax2)
df.plot(y=['diff'], ax=ax3)

# do this bit AFTER the plot
ax3.axhline(threshold, linewidth=1, color='r')
ymin, ymax = ax2.get_ylim()
ax2.vlines(highlights['datetime'], linewidth=1, ymin=ymin, ymax=ymax-1, color='r')

plt.tight_layout()
plt.show()
