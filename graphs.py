import matplotlib.pyplot as plt
import numpy as np

# U values (size of the VEB Tree)
U_values = [//add values here]

# Measured times for insertion (in ns)
insert_times = [//add values here]

# Measured times for successor (in ns)
successor_times = [//add values here]

loglog_U = [np.log2(np.log2(U)) for U in U_values]
plt.figure(figsize=(10, 6))
plt.plot(loglog_U, insert_times, label='Insert Times (Measured)', marker='o', color='blue', linestyle='-', markersize=8)
plt.plot(loglog_U, successor_times, label='Successor Times (Measured)', marker='o', color='red', linestyle='--', markersize=8)

plt.xlabel('log(log(U))', fontsize=12)
plt.ylabel('Time (ns)', fontsize=12)
plt.title('Measured Times vs log(log(U)) for VEB Tree Operations', fontsize=14)


plt.legend()
plt.grid(True)
plt.show()
