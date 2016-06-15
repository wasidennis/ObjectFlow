function extract_CNN_features(dataInfo, dirInfo, para)

%% set up for CNN
caffe_path = fullfile(dirInfo.caffe_folder, 'matlab', 'caffe');
addpath(caffe_path);

model_def_file = [dirInfo.caffe_folder '/examples/pascal_segmentation/fcn-vgg-feature-solver.prototxt'];
use_gpu = true;
mean_pix = [103.939, 116.779, 123.68];
matcaffe_init(use_gpu, model_def_file, []);
weights0 = caffe('get_weights');

load([dirInfo.caffe_folder '/examples/pascal_segmentation/fcn-8s-pascal-weights.mat']);
for i=1:15, weights0(i).weights = weights(i).weights; end
caffe('set_weights', weights0);
layers = para.layers;
scales = para.scales;

%% extract CNN features
cnnSave = [dirInfo.cnnPath '/' dataInfo.videoName(1:end-1) '/'];
if ~exist(cnnSave,'dir'), mkdir(cnnSave); end;
for ff = 1:dataInfo.totalFrame
    path = [cnnSave sprintf('/%05d.mat',ff)];
    if exist(path,'file')
        continue;
    end
    
    im0 = dataInfo.videoAll{ff};
    im = single(im0(:,:,[3 2 1]));
    [im_h, im_w, im_c] = size(im);
    for c = 1:3, im(:,:,c) = im(:,:,c) - mean_pix(c); end
    
    if im_w > im_h && (im_w >= 512 || im_h >= 512)
        left = 0; right = 0;
        top = floor((im_w-im_h)/2); bottom = im_w-im_h-top;
        pad = [top, bottom, left, right];
        im = imPad(im, pad, 0);
        im = imresize(im, [512 512]);
    elseif im_h > im_w && (im_w >= 512 || im_h >= 512)
        left = floor((im_h-im_w)/2); right = im_h-im_w-left;
        top = 0; bottom = 0;
        pad = [top, bottom, left, right];
        im = imPad(im, pad, 0);
        im = imresize(im, [512 512]);
    elseif im_w <= 512 && im_h <= 512
        left = floor((512-im_w)/2); right = 512-im_w-left;
        top = floor((512-im_h)/2); bottom = 512-im_h-top;
        pad = [top, bottom, left, right];
        im = imPad(im, pad, 0);
    end
    
    [im_h,im_w,im_c] = size(im);
    im = reshape(im, [im_h,im_w,im_c,1]);
    im = permute(im,[2,1,3,4]);
    output = caffe('forward', {im});
    acts = caffe('get_all_data');
    
    feats = cell(length(layers),1);
    for j = 1:length(layers),
        act = acts(layers(j)).data;
        [n1,n2,n3] = size(act);
        act = reshape(permute(act,[3,2,1]),[n3,n2*n1]);
        feats{j} = act;
    end
    
    save(path, 'feats', 'pad');
end