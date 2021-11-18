% load MM simulation
load p.txt
load q.txt
load Y.txt
load cx.txt
load cy.txt

figure,
surf(cx, cy, log10(p))
axis equal
shading interp