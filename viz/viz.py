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
df = df.append(read_csv('../output/datr20190913.csv', names=column_names, parse_dates=True, infer_datetime_format=True))
df = df.append(read_csv('../output/datr20190914.csv', names=column_names, parse_dates=True, infer_datetime_format=True))

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

print(df)
# produce a series, based on the results of the large_window median filter,

df['pct_change'] = df[small_window['name']].pct_change()
df['diff'] = df[small_window['name']].diff(periods=1)

threshold = 1200.0
df['thresholded'] = (df['diff'] > threshold) * 1
# that looks for large changes that might be interesting.
# df['diff'] = df['diff'].apply(lambda x: [ if y > threshold else 0 for y in x])
df['high_points'] = df['diff'] > threshold
df['high_points'] = df['high_points'].astype(float)

print(df.info())

# Type hinting. Not sure if this is kosher or not.
fig1: Figure
ax: Axes
fig1, ax = plt.subplots()
# df.plot(x='datetime', y=[small_window['name'], 'high_points'], secondary_y=['high_points'], ax=ax)
df.plot(x='datetime', y=['thresholded', 'diff', small_window['name'] ], secondary_y=['thresholded'], ax=ax)
ax.grid(True, which='major')


plt.tight_layout()
plt.show()
