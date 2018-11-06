def f(n):
    if n < 2:
        return 1
    else:
        return n*f(n-1)

def bi(n,k):
    return f(n)/(f(n-k)*f(k))


def exp(n,l):
    s = sum(sum(bi(n+l,n-l-k-1)*bi(n+l-1,n-l-kk)*((-1)**(k+kk))*(f(2*l+k+kk+2)/(f(k)*f(kk))) for k in range(n-l)) for kk in range(n-l+1))
    return (1/4)*(f(n-l-1)/f(n+l-1))*(((n-l)/(n+l))**.5)*s

def full(n,l,m):
    s = sum(sum(bi(n+l,n-l-k-1)*bi(n+l-1,n-l-kk)*((-1)**(k+kk))*(f(2*l+k+kk+2)/(f(k)*f(kk))) for k in range(n-l)) for kk in range(n-l+1))
    return (1/4)*(f(n-l-1)/f(n+l-1))*((((n-l)/(n+l))*((l*l-m*m)/(4*l*l-1)))**.5)*s


n = 5
for l in range(1,n):
    for m in range(-l+1,l):
        print("<"+str(n)+str(l-1)+str(m)+"|H|"+str(n)+str(l)+str(m)+"> = " + str(full(n,l,m))+"(e*E*a0)")


