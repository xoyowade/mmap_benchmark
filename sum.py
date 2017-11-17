import pandas as pd

import sys

filename = sys.argv[1]

df = pd.read_csv(filename, usecols=['name', 'real_time'])
df = df.groupby('name', as_index=False).mean().sort('real_time')

#print df
df.to_csv(filename, float_format='%.3f', index=False)
