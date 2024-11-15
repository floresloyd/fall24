import os

def list_files_in_directory(directory_path):
    # List all files in the given directory
    try:
        files = os.listdir(directory_path)
        print("Files in directory:", directory_path)
        for file in files:
            print(file)
    except FileNotFoundError:
        print("The specified directory does not exist.")
    except NotADirectoryError:
        print("The specified path is not a directory.")
    except PermissionError:
        print("Permission denied for accessing the directory.")



directory_path = '../../../../../Downloads/nlp-hw2/movie-review-HW2/aclImdb/test/'
list_files_in_directory(directory_path)