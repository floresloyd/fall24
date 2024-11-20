from NB import nb
import os 
import time
import math 
from preprocess import preprocess
from methods import compute_prior_probabilities, compute_conditional_probabilities_with_add1, validate_predictions, get_validation_labels
# ###############  TEST FILES  ####################################
# train_filepath = "./movie-review-small-data/train"
# test_filepath = "./movie-review-small-data/test"
# class2_folder_path = "./movie-review-small-data/test/comedy"
# class1_folder_path = "./movie-review-small-data/test/action"
# actual_labels = "./labels.txt"
# prediction_file = "./predictions.txt"
# output_file = "./labels.txt"

# # Train and Predict NB
# nb(train_filepath, test_filepath)
# # Get Labels
# get_validation_labels(class1_folder_path, class2_folder_path, output_file)
# #Validate
# print()
# validate_predictions(actual_labels, prediction_file)


############### ACTUAL FILES #######################################
## Driver Code 

# Train and Test File Paths
train_filepath = "../../../../../Downloads/nlp-hw2/movie-review-HW2/aclImdb/train"
test_filepath = "../../../../../Downloads/nlp-hw2/movie-review-HW2/aclImdb/test"
# Classes to predict will be used for labels
class2_folder_path = "../../../../../Downloads/nlp-hw2/movie-review-HW2/aclImdb/test/pos"
class1_folder_path = "../../../../../Downloads/nlp-hw2/movie-review-HW2/aclImdb/test/neg"
# Output Files
actual_labels = "./labels.txt"
prediction_file = "./predictions.txt"
output_file = "./labels.txt"

# Train and Predict NB
nb(train_filepath, test_filepath)
# Get Labels
get_validation_labels(class1_folder_path, class2_folder_path, output_file)
#Validate
print()
validate_predictions(actual_labels, prediction_file)