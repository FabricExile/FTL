import json

data = {}
for i in range(1000):
  array = []
  data['array'+str(i)] = array
  for j in range(200):
    obj = {}
    obj['width'] = 10.0
    obj['height'] = 16.0
    obj['text'] = "Hello World"
    array.append(obj)

text = json.dumps(data, indent=2)
with open('test.json', 'w') as f:
  f.write(text)
