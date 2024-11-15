import os
import re
import time


def preprocess(data_path):
    # Initialize a dictionary to store word frequencies {word : count}
    print("Preprocessing files in:", data_path)
    start_time = time.time() # record time
    bow_dict = {}

    # Traverse through all files in the given directory and its subdirectories
    for root, _, files in os.walk(data_path):
        for file in files:
            # Construct the file path
            file_path = os.path.join(root, file)
            # Read the content of the file
            with open(file_path, 'r', encoding='utf-8') as f:
                text = f.read()

            # Pre-process the text: lowercase, tokenize, and remove punctuation
            words = re.findall(r'\b\w+\b', text.lower())  # Tokenizes words, ignores punctuation, and lowercases

            # Update the hashmap with word counts
            for word in words:
                if word in bow_dict:
                    bow_dict[word] += 1
                else:
                    bow_dict[word] = 1

    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"preprocessing() executed in {elapsed_time:.4f} seconds")
    return bow_dict