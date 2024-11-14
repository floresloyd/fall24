import os
import re

def preprocess(data_path):
    # Initialize a dictionary to store word frequencies {word : count}
    print("Processing files in:", data_path)
    bow_dict = {}

    # Traverse through all files in the given directory and its subdirectories
    for root, _, files in os.walk(data_path):
        print()
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

    return bow_dict