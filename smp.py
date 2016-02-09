__author__ = 'xtb269'

###############
#
#  https://en.wikipedia.org/wiki/Stable_marriage_problem#Algorithm
#  SMP algorithm
#
###############
import csv
from collections import OrderedDict
import pprint

def read_inputs(file_name):

    """read inputs from file, return  """
    repo = OrderedDict()
    #repo = {}

    with open(file_name, 'r') as f:
        # Specify delimiter for reader.
        r = csv.reader(f, delimiter=",")

        for row in r:
            row = [s.strip() for s in row]
            name = row.pop(0).strip()
            repo[name] = row

    return repo

def match(repo):

    """
    :param repo
    :return: void
    """
    candidate = repo.keys() [0: len(repo)/2]
    marked = []
    matches = {}
    pp = pprint.PrettyPrinter(indent=2)
    while candidate:
        free_man = candidate.pop()
        print free_man
        for proposed in repo[free_man]:
            if free_man + "," + proposed not in marked:
                if proposed.strip() in matches:
                    other_man = matches[proposed]
                    if repo[proposed].index(free_man) < repo[proposed].index(other_man):
                        matches[proposed] = free_man
                        candidate.append(other_man)
                    else:
                        continue
                else:
                    matches[proposed] = free_man
                marked.append(free_man + "," + proposed)
                break
            else:
                continue
    pp.pprint(matches)

if __name__ == '__main__':
    repo = read_inputs("/Users/xtb269/PycharmProjects/Utils/inputs1.csv")
    match(repo)

