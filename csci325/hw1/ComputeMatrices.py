# you should fill in the functions in this file,
# do NOT change the name, input and output of these functions

import numpy as np
import time
import matplotlib.pyplot as plt

# first function to fill, compute distance matrix using loops
def compute_distance_naive(X):
    N = X.shape[0]      # num of rows
    D = X[0].shape[0]   # num of cols
    
    M = np.zeros([N,N]) # instantiate an array of size N  (# rows) 
    for i in range(N):
        for j in range(N): # loop through all the pairs
            xi = X[i,:]
            xj = X[j,:]
            dist = np.sqrt(np.sum((xi - xj)**2))  # Euclidean distance formula  
            M[i,j] = dist                         # Store distance 

    return M

# second function to fill, compute distance matrix without loops
def compute_distance_smart(X):
    # Step 1: Compute the squared norms of each row | (Sum of squares along axis 1)
    squared_norms = np.sum(X ** 2, axis=1)  # Shape: (N,)

    # Step 2: Compute the dot product of X with its transpose
    dot_product = np.dot(X, X.T)  # Shape: (N, N)

    # Step 3: Use the squared distance formula | Compute norms and dot product for the entire matrix
    dist_squared = squared_norms[:, None] + squared_norms[None, :] - 2 * dot_product

    # Fix: Clip small negative values to zero due to floating-point precision errors
    dist_squared = np.clip(dist_squared, 0, None)

    # Step 4: Take the square root to get Euclidean distance
    M = np.sqrt(dist_squared)

    return M

# third function to fill, compute correlation matrix using loops
def compute_correlation_naive(X):
    N = X.shape[0]        # num of rows
    D = X[0].shape[0]     # num of cols

    # use X to create M
    M = np.zeros([D, D])  # create a (column x column) correlation matrix

    for i in range(D):
        for j in range(D):
            xi = X[:, i]
            xj = X[:, j]

            '''
            Pearson correlation formula:
                r_ij = Σ((x_i - mean(x_i)) * (x_j - mean(x_j))) / sqrt(Σ((x_i - mean(x_i))^2) * Σ((x_j - mean(x_j))^2))

            Where:
                x_i and x_j are the data points for the two variables,    
                mean(x_i) and mean(x_j) are the means of the respective variables,
                r_ij is the correlation coefficient between variables x_i and x_j.
            '''

            # Step 1: Calculate the means of xi and xj
            mean_xi = np.mean(xi)
            mean_xj = np.mean(xj)

            # Step 2: Calculate derivations from the mean 
            xi_dev = xi - mean_xi 
            xj_dev = xj - mean_xj 

            # Step 3: Calculate the correlation coefficient (Pearson)
            numerator = np.sum(xi_dev * xj_dev)
            denominator = np.sqrt(np.sum(xi_dev ** 2) * np.sum(xj_dev ** 2))

            # Step 4: Avoid division by zero in case of constant vectors
            corr = numerator / denominator if denominator != 0 else 0
            
            # you have to change it to correlation between xi and xj
            M[i, j] = corr

    return M

# fourth function to fill, compute correlation matrix without loops
def compute_correlation_smart(X):
    # Center the data by subtracting the mean of each column
    X_centered = X - np.mean(X, axis=0)
    
    # Compute the covariance matrix
    cov_matrix = np.dot(X_centered.T, X_centered) / (X.shape[0] - 1)
    
    # Compute the standard deviation for each column
    std_devs = np.std(X, axis=0, ddof=1)
    
    # Compute the correlation matrix
    M = cov_matrix / np.outer(std_devs, std_devs)
    
    return M

def main():
    print("starting comparing distance computation .....")
    np.random.seed(100)
    params = range(10,141,10)   # different param setting
    nparams = len(params)       # number of different parameters

    perf_dist_loop = np.zeros([10,nparams])  # 10 trials = 10 rows, each parameter is a column
    perf_dist_cool = np.zeros([10,nparams])
    perf_corr_loop = np.zeros([10,nparams])  # 10 trials = 10 rows, each parameter is a column
    perf_corr_cool = np.zeros([10,nparams])

    counter = 0

    for ncols in params:
        nrows = ncols * 10

        print("matrix dimensions: ", nrows, ncols)

        for i in range(10):
            X = np.random.rand(nrows, ncols)   # random matrix

            # compute distance matrices
            st = time.time()
            dist_loop = compute_distance_naive(X)
            et = time.time()
            perf_dist_loop[i,counter] = et - st              # time difference

            st = time.time()
            dist_cool = compute_distance_smart(X)
            et = time.time()
            perf_dist_cool[i,counter] = et - st

            assert np.allclose(dist_loop, dist_cool, atol=1e-06) # check if the two computed matrices are identical all the time

            # compute correlation matrices
            st = time.time()
            corr_loop = compute_correlation_naive(X)
            et = time.time()
            perf_corr_loop[i,counter] = et - st              # time difference

            st = time.time()
            corr_cool = compute_correlation_smart(X)
            et = time.time()
            perf_corr_cool[i,counter] = et - st

            assert np.allclose(corr_loop, corr_cool, atol=1e-06) # check if the two computed matrices are identical all the time

        counter = counter + 1

    mean_dist_loop = np.mean(perf_dist_loop, axis = 0)    # mean time for each parameter setting (over 10 trials)
    mean_dist_cool = np.mean(perf_dist_cool, axis = 0)
    std_dist_loop = np.std(perf_dist_loop, axis = 0)      # standard deviation
    std_dist_cool = np.std(perf_dist_cool, axis = 0)

    plt.figure(1)
    plt.errorbar(params, mean_dist_loop[0:nparams], yerr=std_dist_loop[0:nparams], color='red',label = 'Loop Solution for Distance Comp')
    plt.errorbar(params, mean_dist_cool[0:nparams], yerr=std_dist_cool[0:nparams], color='blue', label = 'Matrix Solution for Distance Comp')
    plt.xlabel('Number of Cols of the Matrix')
    plt.ylabel('Running Time (Seconds)')
    plt.title('Comparing Distance Computation Methods')
    plt.legend()
    plt.savefig('CompareDistanceCompFig.pdf')
    # plt.show()    # uncomment this if you want to see it right way
    print("result is written to CompareDistanceCompFig.pdf")

    mean_corr_loop = np.mean(perf_corr_loop, axis = 0)    # mean time for each parameter setting (over 10 trials)
    mean_corr_cool = np.mean(perf_corr_cool, axis = 0)
    std_corr_loop = np.std(perf_corr_loop, axis = 0)      # standard deviation
    std_corr_cool = np.std(perf_corr_cool, axis = 0)

    plt.figure(2)
    plt.errorbar(params, mean_corr_loop[0:nparams], yerr=std_corr_loop[0:nparams], color='red',label = 'Loop Solution for Correlation Comp')
    plt.errorbar(params, mean_corr_cool[0:nparams], yerr=std_corr_cool[0:nparams], color='blue', label = 'Matrix Solution for Correlation Comp')
    plt.xlabel('Number of Cols of the Matrix')
    plt.ylabel('Running Time (Seconds)')
    plt.title('Comparing Correlation Computation Methods')
    plt.legend()
    plt.savefig('CompareCorrelationCompFig.pdf')
    # plt.show()    # uncomment this if you want to see it right way
    print("result is written to CompareCorrelationCompFig.pdf")

if __name__ == "__main__": main()
