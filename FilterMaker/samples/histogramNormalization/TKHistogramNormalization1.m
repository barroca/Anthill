function TKHistogramNormalization1(inimage, inimagemask, datasetpath, datasetname, mag, targetid)

bins = 0:256;

filepath = strcat(datasetpath, '/', datasetname, '/', mag, 'x/');
maskpath = strcat(datasetpath, '/', datasetname, '/', mag, 'xTissueOnly/');
ofilepath = strcat(datasetpath, '/', datasetname, '/', mag,'xColorAdjusted/');

targetfilename = strcat(filepath, datasetname, '_', targetid, '.aci.', mag, '.png');
img1 = imread(targetfilename, 'png');

maskfilename = strcat(maskpath, datasetname, '_', targetid, '.tissue.', mag, '.png');
fg1 = imread(maskfilename, 'png');
labelind1 = find(fg1>0);
clear fg1;

r1 = img1(:,:,1);
red1 = r1(labelind1);
redhist1 = histc(red1, bins);
clear r1;
clear red1;

g1 = img1(:,:,2);
green1 = g1(labelind1);
greenhist1 = histc(green1, bins);
clear g1;
clear green1;

b1 = img1(:,:,3);
blue1 = b1(labelind1);
bluehist1 = histc(blue1, bins);
clear b1;
clear blue1;

clear labelind1;
clear img1;

fg2 = inimagemask;
labelind2 = find(fg2> 0);
clear fg2;

img2 = inimage;

newimg2 = zeros(size(img2), 'uint8');

r2 = img2(:,:,1);
red2 = r2(labelind2);
r2(labelind2) = uint8(histeq(red2, redhist1));
newimg2(:,:,1) = r2;
clear red2;
clear r2;

g2 = img2(:,:,2);
green2 = g2(labelind2);
g2(labelind2) = uint8(histeq(green2, greenhist1));
newimg2(:,:,2) = g2;
clear green2;
clear g2;

b2 = img2(:,:,3);
blue2 = b2(labelind2);
b2(labelind2) = uint8(histeq(blue2, bluehist1));
newimg2(:,:,3) = b2;
clear blue2;
clear b2;

clear labelind2;

persistent filenum;
if isempty(filenum)
    filenum = 0;
end
filenum = filenum + 1;

outimagename = strcat(ofilepath, datasetname, '_', num2str(filenum), '.aci.', mag, '.png');
disp(outimagename);
imwrite(newimg2, outimagename, 'png');

clear newimg2;

clear redhist1;
clear greenhist1;
clear bluehist1;
