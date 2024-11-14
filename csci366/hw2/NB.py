from preprocess import preprocess
from methods import compute_prior_probabilities

# PRIOR PROB
train_data_path = "./hw2/movie-review-small-data/train"  
prior_probs = compute_prior_probabilities(train_data_path)
print("Prior Probabilities:", prior_probs)

action_data_path = "./hw2/movie-review-small-data/train/action"
action_bow = preprocess(action_data_path)
print(f"Action BOW: {action_bow}")



comedy_data_path = "./hw2/movie-review-small-data/train/comedy"
comedy_bow = preprocess(comedy_data_path)
print(f"Comedy BOW: {comedy_bow}")


'''
TO DO 
1. add time logging to scripts to measure time to run script preprocess
2. figure out how to get probabilities from bow features --> create get_probs() method; figure out how to actually do it from notes
3. predict() method -> argmax{p(comedy), p(action)} --> find a way that i input the hashmaps and it will do it in a modular way 
4. test on actual data.
 

'''