import requests

NB_REQUESTS = 10

# sum = 0
# for i in range(NB_REQUESTS):
#     response = requests.get("http://127.0.0.1:5555/blog")
#     sum += response.elapsed.total_seconds()

# print(sum/NB_REQUESTS)


from matplotlib import pyplot as plt


# time total request (with transport)
x = [100, 500, 1500, 2000] # size of whitelist
y_list = [3, 10, 36, 38] # ms
y_trie = [3, 3, 3, 3] # ms

plt.plot(x, y_list, label="List")
plt.plot(x, y_trie, label="Trie")

plt.legend()
plt.title("Travel time measurements for list and trie")
plt.xlabel("Whitelist size (#lines)")
plt.ylabel("Time (ms)")
plt.show()

# time local
x = [1500] # size of whitelist
y_list = [31]
y_trie = [1]

# it can be imaginable for huge infrastructure to have vast and automatic whitelist generation (10k ?)