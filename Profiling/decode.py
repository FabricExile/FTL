import json

text = open('test.json', 'r').read()
data = json.loads(text)
for i in range(1000):
  array = data['array'+str(i)]
  for j in range(200):
    obj = array[j]
    assert obj['width'] == 10.0
    assert obj['height'] == 16.0
    assert obj['text'] == "Hello World"
