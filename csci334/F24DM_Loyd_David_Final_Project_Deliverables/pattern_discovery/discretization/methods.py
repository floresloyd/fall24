import numpy as np
from sklearn.cluster import KMeans


def discretize_continuous_variables(values, n_bins=5):
    """
    Bins continuous variables into n_bins using k-means clustering.

    Parameters:
        values (list or numpy array): The array of continuous variables to be binned.
        n_bins (int): The number of bins to partition the data into.
    
    Returns:
        dict: A dictionary with bin edges and the bin labels for each value.
    """

    # Step 1: Convert the input to a numpy array
    values = np.array(values).reshape(-1, 1)
    
    # Step 2: Apply k-means clustering to group data into bins
    kmeans = KMeans(n_clusters=n_bins, random_state=42)
    labels = kmeans.fit_predict(values)
    
    # Step 3 Sort cluster centers / Centroids and assign labels to bins based on center ordering
    sorted_indices = np.argsort(kmeans.cluster_centers_.flatten())
    sorted_indices = list(sorted_indices)  # Convert to list for index lookup
    label_to_bin = {original_label: sorted_indices.index(original_label) for original_label in range(n_bins)}
    
    # Step 4: Re-map labels to ordered bin indices
    binned_labels = [label_to_bin[label] for label in labels]                                    # Bin Values The bin labels assigned to each value in the input
    
    # Step 5: Construct bin edges / Clusters 
    sorted_centers = sorted(kmeans.cluster_centers_.flatten())
    bin_edges = [(sorted_centers[i], sorted_centers[i+1]) for i in range(len(sorted_centers)-1)]  # List of tuples where each tuple represents the range between two consecutive cluster centers
    
    return {
        "bin_edges": bin_edges,
        "binned_values": binned_labels
    }


def find_optimal_bin_size(values, max_bins=10):
    """
    The find_optimal_bin_size function aims to determine the best number of bins for discretizing continuous data using the Elbow Method. 
    This method involves clustering data points and finding a point (the "elbow") where further clustering yields diminishing returns.
    The elbow  is where the WCSS (Within-Cluster Sum of Squares) begins to slow down significantly. WCSS represents the total squared
    distance between each data point and the centroid of the cluster to which it belongs.
    
    Parameters:
        values (list or numpy array): The continuous data to bin.
        max_bins (int): Maximum number of bins to consider. We don't want it too be too high to decrease the number of possible patterns within our data

    Returns:
        int: The optimal number of bins.
    """

    # Step 1: Convert into Numpy array for ease of processing
    values = np.array(values).reshape(-1, 1)

    # Step 2: Declare an Array Storage array
    wcss = []  # Within-cluster sum of squares

    # Step 3: Iterate from 1 - 10 to see the best possible cluster 
    for k in range(1, max_bins + 1):
        # Step 4: Cluster Using Kmeans
        kmeans = KMeans(n_clusters=k, random_state=42)
        kmeans.fit(values)
        # Step 5: Per Cluster calculate WCSS
        wcss.append(kmeans.inertia_) 

    # Step 6: Find the elbow point (knee detection)
    deltas = np.diff(wcss)
    second_deltas = np.diff(deltas) # Second derivative 
    optimal_bins = np.argmax(second_deltas < 0) + 2  # Creates a boolean array where values indicate when the second derivative becomes negative (indicating a "knee" in the curve).
    
    # Step 7: Return the first True value or the best split 
    return optimal_bins         



# # Example usage
# start_time = time.time()
# print("running ...")
# # values = [ 1,  3,  2,  4,  5, 13, 12,  9,  7, 10,  6, 11,  8, 14]
# # values = [ 1, 18, 13, 16,  8, 21, 12, 28, 17, 11, 15, 31,  2, 23, 19,  7, 20,
# #        14, 10, 22,  9, 27, 25,  4, 32,  6, 30, 26, 24, 33,  5, 39,  3, 29,
# #        61, 40, 46, 41, 36, 34, 35, 50, 43, 42, 37, 51, 38, 45, 54, 52, 49,
# #        62, 55, 47, 44, 53, 48, 57, 59, 56, 60, 63, 58, 70, 67, 64, 69, 65,
# #        68, 66, 81, 79, 75, 72, 74]

# values = [ 1,  9,  6,  7,  5,  8,  3,  4,  2, 16, 12, 13, 15, 10, 11, 14]

# binsize = find_optimal_bin_size(values)
# print(f"Optimal binsize: {binsize}")
# result = bin_continuous_variables(values, binsize)
# print(result['bin_edges'])
# print(result["binned_values"])