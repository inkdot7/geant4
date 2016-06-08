hm.selectStore("brachy3.hbk")
hm.ls()
#spectrum of initial particles      
hEn=hm.load1D("30") 
hplot(hEn)
#if you want to print the results:pl.psPrint("Brachyspettro.ps") 
h2=hm.load2D("20")
hplot(h2)
v=vm.from2D(h2)
vm.list()      
vector=vm.retrieve2D (3)  
pl.dataOption("representation","color")
pl.plot(vector)
vector.toAscii("brachy.dat")
pl.reset()