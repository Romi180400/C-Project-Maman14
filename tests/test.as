.entry foo
.extern foo
.extern foo itzik
.ext itzik
.ent shlomi
D: .data 1,2,3,foo,shlomi,itzik
F: .string "Hello World"  
A: mov #3,r2
B: jmp r3
mov STR[5],STR[3]