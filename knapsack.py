# http://www.geeksforgeeks.org/dynamic-programming-set-10-0-1-knapsack-problem/
# A Dynamic Programming based Python Program for 0-1 Knapsack problem
# Returns the maximum value that can be put in a knapsack of capacity W
# W: weight limit
# wt: weight array
# val: value array
# n: number of items
def knapSack(W, wt, val, n):
    K = [[0 for x in range(W+1)] for x in range(n+1)]
    r = [[0  for x in range(W+1)] for x in range(n+1)]
 
    # Build table K[][] in bottom up manner
    for i in range(n+1):
        for w in range(W+1):
            if i==0 or w==0:
                K[i][w] = 0
            elif wt[i-1] <= w:
                K[i][w] = max(val[i-1] + K[i-1][w-wt[i-1]],  K[i-1][w])
                if ( val[i-1] + K[i-1][w-wt[i-1]] > K[i-1][w] ):
                    r[i][w] = 1
            else:
                K[i][w] = K[i-1][w]
    kk = W
    output = []
    for i in reversed(range(n)):
        if r[i][kk] == 1:
            output.append(val[i])
            kk = kk - wt[i]

 
    return K[n][W],output


if __name__ == "__main__":
 
    val = [5,3,9,2,18,8,2,3,3,16,13,7,14,16,14,17,16,4,1,3,1]
    wt = [ 1 for x in range(len(val))]
    W = 5
    n = len(val)
    while ( len(val) > 5 ):
       (final, output) = knapSack(W, wt, val, n)
       print(output)
       
       for i in output:
           val.remove(i)
       n = len(val)

    print(val)
