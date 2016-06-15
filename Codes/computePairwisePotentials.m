rSource = [];
rDestination = [];
value = [];
numSuperpixelNode = size(unarySupervoxelPotentials,1);
for kk = 1:nFrames
    subSuperpixel = subSuperpixels{kk} - 1;
    subPixel = subPixels{kk} + numSuperpixelNode - 1;
    
    for ll = subSuperpixelsNum(kk)+1:subSuperpixelsNum(kk+1)
        idx = subPixel(subSuperpixel==ll); idx(idx==numSuperpixelNode) = [];
        rSource = [rSource;ll*ones(length(idx),1)];
        rDestination = [rDestination;idx];
        tmp = 1 - abs( fgProPixel{kk}(idx-subPixelsNum(kk)-numSuperpixelNode) - fgProSupervoxel{kk}(ll-subSuperpixelsNum(kk)) ) ;
        value = [value;tmp];
    end
end
relationPotentials.source = uint32(rSource-1);
relationPotentials.destination = uint32(rDestination-1);
relationPotentials.value = single([value*para.supervoxel2pixelWeight value*para.pixel2supervoxelWeight]);