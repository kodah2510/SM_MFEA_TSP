import numpy as np
from matplotlib import pyplot as plt
def plot_front(file):
    X = [];
    Y = []
    for line in file:
        data = line.split(' ')
        X.append(float(data[0]))
        Y.append(float(data[1]))
        pass
    X = np.array(X)
    Y = np.array(Y)

    plt.scatter(X, Y)
    plt.show()
    pass

def plot_graph(f_smmfea, f_soea, f_moea):
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
    y_smmfea = np.array(y_smmfea)
    y_soea   = np.array(y_soea)
    y_moea   = np.array(y_moea)

    x = np.arange(100)
    
    plt.plot(x, y_smmfea)
    plt.plot(x, y_soea)
    plt.plot(x, y_moea)

    plt.show()
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
                        "pr226",    "pr264", "pr299","rat195"]
    #"pr264","pr299","rat195"
    #convergence trend
   
    for pn in problem_name:
        dv_smmfea   = open(an[0] + '\\' + an[0] + '_' + pn +'_dv.txt', 'r')
        dv_soea     = open(an[1] + '\\' + an[1] + '_' + pn +'_dv.txt', 'r')
        dv_moea     = open(an[2] + '\\' + an[2] + '_' + pn +'_dv.txt', 'r')
        plot_graph(dv_smmfea, dv_soea, dv_moea)

        front_smmfea = open(an[0] + '\\' + an[0] + '_' + pn+'_front.txt', 'r')
        front_moea   = open(an[2] + '\\' + an[2] + '_' + pn+'_front.txt', 'r')
        plot_front(front_smmfea)
        plot_front(front_moea)
        pass
    pass
   


if __name__ == '__main__':
    main()