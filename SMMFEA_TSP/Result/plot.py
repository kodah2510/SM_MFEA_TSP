import numpy as np
from matplotlib import pyplot as plt
import matplotlib.ticker as mticker
class MathTextSciFormatter(mticker.Formatter):
    def __init__(self, fmt="%1.2e"):
        self.fmt = fmt
    def __call__(self, x, pos=None):
        s = self.fmt % x
        decimal_point = '.'
        positive_sign = '+'
        tup = s.split('e')
        significand = tup[0].rstrip(decimal_point)
        sign = tup[1][0].replace(positive_sign, '')
        exponent = tup[1][1:].lstrip('0')
        if exponent:
            exponent = '10^{%s%s}' % (sign, exponent)
        if significand and exponent:
            s =  r'%s{\times}%s' % (significand, exponent)
        else:
            s =  r'%s%s' % (significand, exponent)
        return "${}$".format(s)
def plot_front(file, pn, an, i):
    fig = plt.figure(num=pn+'_'+an)
    ax = fig.add_subplot(111)

    X = [];
    Y = []
    for line in file:
        data = line.split(' ')
        X.append(float(data[0]))
        Y.append(float(data[1]))
        pass
    X = np.array(X)
    Y = np.array(Y)

    S = X + Y
    mi = np.argmin(S)
    m = min(S)
    p1 = [m, 0]
    p2 = [0, m]
    #x + y = m
    # y = m - x
    ax.scatter(X, Y, facecolors='none', edgecolors='#376ec6')
    ax.plot(p1, p2, color='red')

    ax.annotate(r'$x+y={0:.2f}$'.format(m), 
                xy=(X[mi], Y[mi]), xytext=(0.3, 0.2),
                textcoords='figure fraction',
                arrowprops=dict(shrink=0.05,facecolor='black', width=0.1, headwidth=0.1))

    ax.set_xlabel(r'$f_1$')
    ax.set_ylabel(r'$f_2$')
    mf = mticker.ScalarFormatter(useMathText=True)
    mf.set_powerlimits((-2,2))
    ax.yaxis.set_major_formatter(mf)
    ax.xaxis.set_major_formatter(mf)
    ax.margins(0)
    #fig.tight_layout()
    plt.savefig(fname='img\\'+an+'_'+pn+'_front_'+str(i)+'.png')
    #plt.show()
    plt.close(fig)
    pass

def plot_graph(f_smmfea, f_soea, f_moea, pn):
    fig = plt.figure(num=pn)
    
    ax = fig.add_subplot(111)

    y_smmfea    = []
    y_soea      = []
    y_moea      = []

    for line in f_smmfea:
        y_smmfea.append(float(line))
        pass
    for line in f_soea:
        y_soea.append(float(line))
        pass
    for line in f_moea:
        y_moea.append(float(line))
        pass

    y_smmfea = np.array(y_smmfea[0::3])
    y_soea   = np.array(  y_soea[0::3])
    y_moea   = np.array(  y_moea[0::3])

    x = np.arange(100)
    x = x[0::3]
    ax.plot(x, y_smmfea, marker='v', 
                         color='red', 
                         label='S&M-MFEA',
                         markerfacecolor='none',
                         linewidth=2)
    ax.plot(x, y_soea,   marker='o', 
                         color='#164a9e', 
                         label='SOEA',
                         markerfacecolor='none',
                         linewidth=2)
    ax.plot(x, y_moea,   marker='',  
                         color='black',  
                         label='MOEA',
                         linewidth=2)
    #ax.margins(0)
    #ax.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    ax.set_ylabel('Total distance')
    ax.set_xlabel('Generations')
    mf = mticker.ScalarFormatter(useMathText=True)
    mf.set_powerlimits((-2,2))
    ax.yaxis.set_major_formatter(mf)
    #ax.get_yaxis().get_offset_text().set_visible(False)

    # Add in a text box at the top of the y axis
    # ax.text(0.0, 1.0, offset_text, transform=ax.transAxes,
    #         horizontalalignment='left',
    #         verticalalignment='bottom')
    #ax.yaxis.set_major_formatter(MathTextSciFormatter())
 
    ax.legend()
    plt.savefig('img\\'+pn+'_dv.png')
    #plt.show()
    plt.close(fig)
    pass
def main():
    # X = np.linspace(-np.pi, np.pi, 256, endpoint=True)
    # C, S = np.cos(X), np.sin(X)

    # plt.plot(X, C)
    # plt.plot(X, S)

    # plt.show()
    an = ["SMMFEA", "SOEA", "MOEA"]

    problem_name    =   ["eil51",    "bier127",  "ch130","kroA150",
                        "kroA200",  "kroB150",  "kroB200","lin105",
                        "pr76",     "pr107",    "pr136","pr144","pr152",
                        "pr226",    "pr264", "pr299","rat195", "ts225", "u159"]
    #"pr264","pr299","rat195"
    #convergence trend
  
    for pn in problem_name:
        dv_smmfea   = open(an[0] + '_' + pn + '_dv.txt', 'r')
        dv_soea     = open(an[1] + '_' + pn + '_dv.txt', 'r')
        dv_moea     = open(an[2] + '_' + pn + '_dv.txt', 'r')
        plot_graph(dv_smmfea, dv_soea, dv_moea, pn)

      
        pass
    pass
    for i in range(1, 6):
        for pn in problem_name:
            front_smmfea = open(an[0] + '\\' + an[0] + '_' + pn + '_front_'+ str(i) +'.txt', 'r')
            front_moea   = open(an[2] + '\\' + an[2] + '_' + pn + '_front_'+ str(i) +'.txt', 'r')
            plot_front(front_smmfea, pn, 'smmfea', i)
            plot_front(front_moea, pn, 'moea', i)
        pass
    pass
    

if __name__ == '__main__':
    main()