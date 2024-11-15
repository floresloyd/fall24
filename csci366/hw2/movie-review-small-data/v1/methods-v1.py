# import os 
# import time
# import re


# def compute_prior_probabilities(data_path):
#     print(f"Computing Prior Probabilities for {data_path} ...")
#     start_time = time.time()  # record time

#     total_docs = 0          # Document counter
#     class_doc_counts = {}   # Storage to store frequency of documents per class

#     # Traverse through all directories (classes)
#     for root, dirs, files in os.walk(data_path):
#         # Skip the root directory and only process class subdirectories
#         if root != data_path:
#             # Get the class name (folder name)
#             class_name = os.path.basename(root)
#             num_docs = len(files)  # Count the number of documents in this class
#             total_docs += num_docs  # Update the total document count
#             class_doc_counts[class_name] = num_docs  # Store the document count for this class

#     # Ensure there are exactly two classes
#     if len(class_doc_counts) != 2:
#         raise ValueError("Expected exactly 2 classes within the data_path.")

#     # Extract class names and their document counts
#     class_names = list(class_doc_counts.keys())
#     num_docs_class1 = class_doc_counts[class_names[0]]
#     num_docs_class2 = class_doc_counts[class_names[1]]

#     # Calculate prior probabilities for each class
#     prior_prob1 = num_docs_class1 / total_docs if total_docs > 0 else 0
#     prior_prob2 = num_docs_class2 / total_docs if total_docs > 0 else 0

#     end_time = time.time()
#     elapsed_time = time.time() - start_time
#     print(f"compute_prior_probabilities() executed in {elapsed_time:.4f} seconds")

#     return class_names[0], prior_prob1, class_names[1], prior_prob2


# def compute_conditional_probabilities_with_add1(bow1, bow2):
#     print(f"Computing conditional probabilities and smoothing ...")
#     start_time = time.time() # record time
    
    
#     bow1_size = 0
#     conditional_probabilities1 = {}

#     bow2_size = 0
#     conditional_probabilities2 = {}

#     # count total number of tokens in bow1
#     for token in bow1:
#         bow1_size += bow1[token]
#     print(f"There are {bow1_size} number of tokens in BOW1")


#     # count total number of tokens in bow1
#     for token in bow2:
#         bow2_size += bow2[token]
#     print(f"There are {bow2_size} number of tokens in BOW2")


#     vocabulary_size = bow1_size + bow2_size
#     print(f"There are {vocabulary_size} number of tokens in the vocabulary of the Folder")

#     # compute conditional probabilities for bow1 and bow2
#     for token in bow1:
#         conditional_probabilities1[token] = (bow1[token] + 1) / (bow1_size + vocabulary_size)

#     for token in bow2:
#         conditional_probabilities2[token] = (bow2[token] + 1) / (bow2_size + vocabulary_size)

#     end_time = time.time()
#     elapsed_time = end_time - start_time
#     print(f"compute_conditional_probabilities_with_add1() executed in {elapsed_time:.4f} seconds")

#     return conditional_probabilities1, bow1_size, conditional_probabilities2, bow2_size, vocabulary_size


# def get_validation_labels(class1_folder_path, class2_folder_path, output_file):
#     start_time = time.time() # record time
#     with open(output_file, "w") as file:
#         file.write("Actual Labels\n")
        
#         # Write for Class 1
#         file.write(f"--1\n")
#         for file_name in os.listdir(class1_folder_path):
#             if os.path.isfile(os.path.join(class1_folder_path, file_name)):
#                 file.write(f"1 {file_name}\n")
        
#         # Write for Class 2
#         file.write(f"--0\n")
#         for file_name in os.listdir(class2_folder_path):
#             if os.path.isfile(os.path.join(class2_folder_path, file_name)):
#                 file.write(f"0 {file_name}\n")
#     end_time = time.time()
#     elapsed_time = end_time - start_time
#     print(f"get_validation_labels() in {elapsed_time:.4f} seconds")




# def validate_predictions(actual_labels_file, predictions_file):
#     start_time = time.time() # record time
#     # Read the actual labels into a list
#     with open(actual_labels_file, "r") as actual_file:
#         actual_lines = [line.strip() for line in actual_file if not line.startswith("Actual Labels") and not line.startswith("--")]

#     # Read the predictions into a list
#     with open(predictions_file, "r") as pred_file:
#         pred_lines = [line.strip() for line in pred_file if not line.startswith("Making predictions") and not line.startswith("--")]

#     correct_predictions = 0
#     total_predictions = 0

#     # Compare actual labels with predictions
#     for actual, prediction in zip(actual_lines, pred_lines):
#         actual_label, actual_filename = actual.split()
#         predicted_label, _ = prediction.split(maxsplit=1)  # Ignore the second column (probability)

#         total_predictions += 1
#         if actual_label == predicted_label:
#             correct_predictions += 1

#     # Calculate accuracy
#     accuracy = (correct_predictions / total_predictions) * 100 if total_predictions > 0 else 0
#     print(f"Validation Completed: {correct_predictions}/{total_predictions} correct predictions.")
#     print(f"Accuracy: {accuracy:.2f}%")
#     end_time = time.time()
#     elapsed_time = end_time - start_time
#     print(f"validate_predictions() in {elapsed_time:.4f} seconds")



# def generate_label_mapping(test_data_path):
#     label_mapping = {}
#     subdirectories = [d for d in sorted(os.listdir(test_data_path)) if os.path.isdir(os.path.join(test_data_path, d))]

#     if len(subdirectories) != 2:
#         raise ValueError("Expected exactly 2 subdirectories (classes) within the test data path.")

#     # Assign labels to the discovered classes
#     class1_folder_path = os.path.join(test_data_path, subdirectories[0])
#     class2_folder_path = os.path.join(test_data_path, subdirectories[1])

#     # Class 1 Mapping (Label 1)
#     for file_name in sorted(os.listdir(class1_folder_path)):
#         if os.path.isfile(os.path.join(class1_folder_path, file_name)):
#             label_mapping[file_name] = 1

#     # Class 2 Mapping (Label 0)
#     for file_name in sorted(os.listdir(class2_folder_path)):
#         if os.path.isfile(os.path.join(class2_folder_path, file_name)):
#             label_mapping[file_name] = 0

#     return label_mapping