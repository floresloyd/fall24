import pandas as pd
import numpy as np
from itertools import combinations
from sklearn.metrics import mutual_info_score
import seaborn as sns
import matplotlib.pyplot as plt
from mlxtend.frequent_patterns import apriori, association_rules
from scipy.stats import chi2_contingency, entropy

# Load data
file_path = 'full_masked_transformed_diabetic_data.csv'
data = pd.read_csv(file_path)

# One-Hot Encode the discretized data for ARM
# This will convert each category in every column into a separate binary column
binary_data = pd.get_dummies(data, prefix_sep='_', columns=data.columns)

# Convert to boolean type to optimize performance
binary_data = binary_data.astype(bool)

# Find frequent itemsets using Apriori
frequent_itemsets = apriori(binary_data, 
                            min_support=0.1,  # minimum support threshold
                            use_colnames=True)

# First, get the number of itemsets from the frequent_itemsets DataFrame
num_itemsets = len(frequent_itemsets)

# Then include it in the association_rules call
rules = association_rules(frequent_itemsets, 
                         num_itemsets=num_itemsets,  # Add this parameter
                         metric="confidence",
                         min_threshold=0.5)

# Sort rules by lift to identify the strongest associations
rules = rules.sort_values('lift', ascending=False)

print("Top Association Rules:")
print(rules.head(10))

# Mutual Information Calculation (for statistical dependencies)
columns = data.columns
n_cols = len(columns)
mi_matrix = np.zeros((n_cols, n_cols))

pairs_with_mi = []
for i, col1 in enumerate(columns):
    for j, col2 in enumerate(columns):
        if i < j:  # Only calculate for unique pairs
            mi = mutual_info_score(data[col1], data[col2])
            mi_matrix[i, j] = mi
            mi_matrix[j, i] = mi  # Matrix is symmetric
            pairs_with_mi.append({
                'column1': col1,
                'column2': col2,
                'mutual_info': mi
            })

# Convert to DataFrame and sort by mutual information
mi_df = pd.DataFrame(pairs_with_mi)
mi_df = mi_df.sort_values('mutual_info', ascending=False)

print("\nTop Statistical Dependencies:")
print(mi_df.head(10))

# Create heatmap for Mutual Information
plt.figure(figsize=(12, 8))
sns.heatmap(mi_matrix, xticklabels=columns, yticklabels=columns, annot=True, fmt='.3f', cmap='YlOrRd')
plt.title('Mutual Information Between Variables')
plt.tight_layout()
plt.savefig('mutual_information_heatmap.png')
plt.close()

# Save detailed results to CSV
mi_df.to_csv('mutual_information_scores.csv', index=False)
print("\nResults saved to 'mutual_information_scores.csv' and heatmap saved to 'mutual_information_heatmap.png'")

def find_significant_associations(data, alpha=0.05):
    columns = data.columns
    significant_pairs = []
    
    for col1, col2 in combinations(columns, 2):
        # Create contingency table
        contingency = pd.crosstab(data[col1], data[col2])
        
        # Perform chi-square test
        chi2, p_value, dof, expected = chi2_contingency(contingency)
        
        if p_value < alpha:
            significant_pairs.append({
                'variables': (col1, col2),
                'chi2': chi2,
                'p_value': p_value,
                'strength': chi2 / (len(data) * min(contingency.shape))
            })
    
    return pd.DataFrame(significant_pairs).sort_values('strength', ascending=False)

significant_associations = find_significant_associations(data)
print("\nStatistically Significant Associations:")
print(significant_associations)

# Calculate normalized mutual information
def normalized_mutual_info(data):
    columns = data.columns
    pairs = []
    
    for col1, col2 in combinations(columns, 2):
        mi = mutual_info_score(data[col1], data[col2])
        # Calculate entropies
        h1 = entropy(data[col1].value_counts(normalize=True))
        h2 = entropy(data[col2].value_counts(normalize=True))
        # Normalize MI
        nmi = mi / min(h1, h2)
        
        pairs.append({
            'variables': (col1, col2),
            'mutual_info': mi,
            'normalized_mi': nmi
        })
    
    return pd.DataFrame(pairs).sort_values('normalized_mi', ascending=False)

normalized_mi_scores = normalized_mutual_info(data)
print("\nNormalized Mutual Information Scores:")
print(normalized_mi_scores)

# After your existing analysis code, add:

# Create a text file with formatted results
with open('analysis_results.txt', 'w') as f:
    f.write("DIABETES DATASET ANALYSIS RESULTS\n")
    f.write("================================\n\n")
    
    f.write("1. TOP ASSOCIATION RULES\n")
    f.write("----------------------\n")
    f.write(f"Total number of frequent itemsets found: {len(frequent_itemsets)}\n")
    f.write("Support threshold: 0.1\n")
    f.write("Confidence threshold: 0.5\n\n")
    f.write("Frequent Itemsets Summary:\n")
    f.write(frequent_itemsets.to_string())
    f.write("\n\nTop 10 Association Rules by Lift:\n")
    f.write(rules.head(10).to_string())
    f.write("\n\n")
    
    f.write("2. MUTUAL INFORMATION SCORES\n")
    f.write("--------------------------\n")
    f.write(mi_df.head(10).to_string())
    f.write("\n\n")
    
    f.write("3. STATISTICALLY SIGNIFICANT ASSOCIATIONS\n")
    f.write("-------------------------------------\n")
    f.write(significant_associations.to_string())
    f.write("\n\n")
    
    f.write("4. NORMALIZED MUTUAL INFORMATION SCORES\n")
    f.write("-----------------------------------\n")
    f.write(normalized_mi_scores.to_string())

# Save to Excel with multiple sheets
with pd.ExcelWriter('analysis_results.xlsx') as writer:
    # Analysis parameters sheet
    pd.DataFrame({
        'Metric': ['Total Frequent Itemsets', 'Support Threshold', 'Confidence Threshold'],
        'Value': [len(frequent_itemsets), 0.1, 0.5]
    }).to_excel(writer, sheet_name='Analysis Parameters')
    
    # Results sheets
    frequent_itemsets.to_excel(writer, sheet_name='Frequent Itemsets')
    rules.head(10).to_excel(writer, sheet_name='Association Rules')
    mi_df.head(10).to_excel(writer, sheet_name='Mutual Information')
    significant_associations.to_excel(writer, sheet_name='Significant Associations')
    normalized_mi_scores.to_excel(writer, sheet_name='Normalized MI')

print("Results have been saved to:")
print("1. 'analysis_results.txt' (text format)")
print("2. 'analysis_results.xlsx' (Excel format with multiple sheets)")
