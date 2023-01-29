import requests

NB_REQUESTS = 10

sum = 0
for i in range(NB_REQUESTS):
    response = requests.get("http://127.0.0.1:5555/blog")
    sum += response.elapsed.total_seconds()

print(sum/NB_REQUESTS)