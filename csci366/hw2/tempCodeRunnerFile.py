###############  TEST FILES  ####################################
train_filepath = "./movie-review-small-data/train"
test_filepath = "./movie-review-small-data/test"
class2_folder_path = "./movie-review-small-data/test/comedy"
class1_folder_path = "./movie-review-small-data/test/action"
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
