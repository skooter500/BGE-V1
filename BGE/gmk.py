import sys

if len(sys.argv) != 2: 
    print 'Usage:', __file__, '<filename>' 
    exit(1)

fid = sys.argv[1].replace('.cpp', '').replace('.h', '')

graph = {fid : []}
i = 0
x = open('main.cpp', 'r').read() 
queue = []
taboo = []

def next_header(index, source): 
    ni = source[index:].find('#include "') 
    offset = ni + len('#include "') 
    if ni < 0: 
        ni = source[index:].find('#include"') 
        offset = ni + len('#include"') 
    if ni < 0: return '', index 
    output = ''
    index += offset 
    for char in source[index:]: 
        if char == '"': return output.replace('.cpp', '').replace('.h', ''), index 
        output += char 

def get_for_file(name): 
    output = [] 
    f = None 
    try: 
        f = open(name + '.cpp', 'r').read() 
    except IOError: pass
    if not f: f = open(name + '.h', 'r').read() 
    i = 0
    header, i = next_header(i, f) 
    while header != '': 
        output.append(header) 
        header, i = next_header(i, f) 
    return output

def update_queue(queue, taboo, incoming): 
    for i in incoming: 
        if i not in taboo: queue.append(i) 
    return list(set(queue)) 


queue = get_for_file(fid)

graph[fid] = queue

while len(queue) > 0: 
    current = queue[0]
    queue = queue[1:] 
    incoming = get_for_file(current) 
    if not graph.has_key(current): graph[current] = incoming
    else: graph[current] = list(set(graph[current] + incoming))
    taboo.append(current) 
    queue = update_queue(queue, taboo, incoming)
    print current, queue, incoming  

print get_for_file('Game')
