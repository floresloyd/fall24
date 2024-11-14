import os 

def compute_prior_probabilities(data_path):
    # Dictionary to store prior probabilities {class_name: prior probability}
    prior_probs = {}
    total_docs = 0          # document counter
    class_doc_counts = {}   # storage to store frequency of documents per class to be used to compute prior probability 

    # Traverse through all directories (classes)
    for root, dirs, files in os.walk(data_path):
        # Skip the root directory and only process class subdirectories
        if root != data_path:
            # Get the class name (folder name)
            class_name = os.path.basename(root)
            num_docs = len(files)  # Count the number of documents in this class
            total_docs += num_docs  # Update the total document count
            class_doc_counts[class_name] = num_docs  # Store the document count for this class

    # Calculate prior probabilities for each class
    for class_name, num_docs in class_doc_counts.items():
        prior_probs[class_name] = num_docs / total_docs if total_docs > 0 else 0

    return prior_probs

