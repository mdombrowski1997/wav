from math import sin, cos, sqrt, pi, floor
import cmath
import matplotlib.pyplot as plt

N = 0x30C
x = [0] * N
contents = ""
# Sig
with open('sig') as f:
    contents = f.read(N)
x = contents.split(',')
N = len(x)
x[311] = '16'
for i in range(N):
    x[i] = int(x[i])
# Square
# for k in range(N):
#     x[k] = k//8 % 2
# Cos
# for k in range(N):
#     x[k] = cos(k*pi/8)
X = [0 + 0j] * N
mags = [0] * (N+1)
angs = [0] * (N+1)
for k in range(N):
    for n in range(N):
        X[k] += x[n]*(cos(2*pi*k*n/N) - sin(2*pi*k*n/N)*1j)
for k in range(len(X)):
    mags[k], angs[k] = cmath.polar(X[k]/N)
mags[k+1] = mags[0]
sum = 0
for i,val in enumerate(mags):
    sum += val/(i+1)
print((sum-mags[k+1]/(k+1))*4/pi)
# Signal plot
# plt.plot(range(N), x, 'r-')
plt.plot(range(N+1), mags, 'g-')
# plt.plot(range(floor(N/4), floor(N/4 + N/2)), mags[:floor(N/2)], 'g-')
# plt.plot(range(floor(-N/4 - N/2), floor(-N/4)), mags[floor(N/2)+1:], 'g-')
plt.show()
