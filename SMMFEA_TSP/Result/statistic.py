import numpy as np
def main():
    folder_name = ["SMMFEA", "SOEA", "MOEA"]
    problem_name = ["eil51", "bier127", "ch130","kroA150",
            "kroA200", "kroB150", "kroB200","lin105",
            "pr76", "pr107", "pr136","pr144","pr152",
            "pr226", "pr264", "pr299", "rat195", "ts225", "u159"]
    
    # dv = []
    # for pn in problem_name:
    #     for fn in folder_name:
    #         dv = []
    #         for j in range(1, 6):
    #             file_link =  fn + '\\' + fn + "_" + pn + '_dv_'+str(j)+ '.txt'
    #             #print(file_link)
    #             f = open(file_link)
    #             res = []
    #             for line in f:
    #                 res.append(float(line))
    #             dv.append(res)
    #             pass
    #         dv = np.array(dv)
    #         average = np.average(dv, axis=0)
    #         output_link = fn + '_' + pn + '_dv.txt'
    #         op_file = open(output_link, 'w')
    #         for e in average:
    #             op_file.write(str(format(e, '.2f')) + '\n')
    #         pass
    #         op_file.close()
    #     pass
    # pass

    try:
        link = ''
        of = open('out.txt', 'w')
        of.write('problem & best known & MFEA & SOEA & MOEA \\\\ \n')
        #of.write('\\hline')
        for pn in problem_name:
            res = []
            for fn in folder_name:
                link = fn + '_' + pn + '_dv.txt'
                print(link)
                f = open(link)
                lines = f.read().splitlines()
                last_line = lines[-1]
                res.append(float(last_line))
                pass
            line = pn + ' & ' + str(min(res)) 
            res_line = ''
            for r in res:
                res_line += '& ' + str(r) 
            line += res_line + ' \\\\ \n'
            of.write(line)
            pass
        of.close()
    except IndexError:
        print('err: ' + link)







if __name__ == '__main__':
    main()