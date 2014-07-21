import os

'''conversions = {'#include<gl/' : '#include <GL/', '#include <gl/' : '#include <GL/'} 
folder = '../BGE' 

contents = os.listdir(folder)   

for f in contents: 
    if not os.path.isfile(os.path.join(folder, f)) and (not f.endswith('.h') or not f.endswith('.cpp')): continue 
    data = open(folder + '/' + f, 'r').read() 
    for x in conversions.keys(): data = data.replace(x, conversions[x]) 
    open(folder + '/' + f, 'w').write(data)
    print f '''


print 'CC=g++'
print 'OPTS=-fpermissive -w -std=gnu++0x'
print 'START=$(CC) $(OPTS)'
print 'DEPS=../Dependancies'
print 'LDEPS=$(DEPS)/linux'
print 'OBJ=-c -o'
print 'PRIMARY=$(START) $(OBJ)' 
print ''
print '# Include valid header files'
print 'IGLM=-I$(DEPS)/glm-0.9.4.3/glm\t\t\t# Include GLM header files'
print 'ISTLTTF=-I$(LDEPS)/SDL2_ttf-2.0.12\t\t# Include STL_ttf header files'
print 'ISTL=-I$(LDEPS)/SDL-2.0.2-8236/include\t\t# Include STL header files'
print 'IFMOD=-I$(LDEPS)/fmodapi44431linux/api/inc\t\t# Include FMOD header files'
print 'IBUL=-I$(LDEPS)/bullet-2.81-rev2613/src\t\t# Include Bullet Physics header files'
print ''
print 'ALLDEPS=$(IGLM) $(ISTLTTF) $(ISTL) $(IFMOD) $(IBUL)' 
print ''

def name(original): return original.lower().replace('.', '_') 

folder = '../BGE' 
files = os.listdir(folder)
types = {} 

for f in files: 
    if os.path.isfile(os.path.join(folder, f)) and (f.endswith('.h') or f.endswith('.hpp') or f.endswith('.cpp')): 
        typename = f.replace('.h', '').replace('.hpp', '').replace('.cpp', '') 
        if not types.has_key(typename): types[typename] = [f] 
        else: types[typename].append(f) 

print '# File groupings' 
for t in types.keys(): 
    names = '' 
    for x in types[t]: names += name(x) + ' ' 
    print t.upper() + '=' + names[:-1] 
print ''

counter = 0
sc = 0 
print 'compile_0:', 
for t in types.keys(): 
    if counter >= 5: 
        counter = 0 
        print '' 
        sc += 1
        print 'compile_' + str(sc) + ':', 
    counter += 1
    print '$(' + t.upper() + ')', 
print '' 
print '' 

for i in range(sc+1): 
    print 'compile_' + str(i), 
print '' 
print ''

for t in types.keys(): 
    for x in types[t]: 
        print name(x) + ':' 
        print '\t$(PRIMARY) ' + name(x) + '.o $(ALLDEPS) ' + folder + '/' + x 
        print '' 

print ''
print 'clean:\n\trm *.o' 
