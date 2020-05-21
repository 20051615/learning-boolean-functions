The aim is to represent each counter variable with a formula containing candidate variables (the term comes from the QSAT-solving technique CEGAR).
Each file contains {nclauses} (usually 128) examples. Files with the same prefix belong to the same problem (and can be treated as one).

A literal is denoted as [-]i (where i is the number of the variable and '-' denotes negation).

File format:
{ncandidate} {ncounter} {nclauses}
l1 l2 l3 ...		# The candidate literals for the first example
k1 k2 k3 ...		# The counter literals for the first example
...			# The remaining examples