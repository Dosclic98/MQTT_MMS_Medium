clear 

%Hidden variables
h_states = {'MITM', 'SRM', 'UC', 'UPS'};
%Observable variables
obs = {'MC', 'CC', 'IMD'};
%Array containing each node name 
names=[h_states, obs];

%Number of nodes 
nNodes=length(names);

onodes = 1:nNodes;

%Intraslice edges
intrac = {'MITM', 'SRM';
'MITM', 'UC';
'MITM', 'IMD';
'SRM', 'UPS';
'SRM', 'MC';
'UC', 'UPS';
'UC', 'CC'};

%Making intraslice adjiacent matrix, names1 reordered according to
%topological order
[intra, names1] = mk_adj_mat(intrac, names, 1);

% Compute the topological order as a permutation of the cellarray names  
perm = arrayfun(@(x) find(strcmp(names,x)),names1);
names = names1;

%Interslice edges
interc = {'MITM', 'MITM';
'SRM', 'SRM';
'UC', 'UC';
'UPS', 'UPS';
};

%Making interslice adjiacent matrix
inter = mk_adj_mat(interc, names, 0);

% Number of states (ns(i)=x means variable i has x states). 
% The vector is defined according to the original order of vector names 
ns = [4 4 4 2 4 4 2];
% Reorder number of states according to topological order computed in perm
ns=ns(perm);

% Creating the DBN
bnet = mk_dbn(intra, inter, ns, 'names', names);

% I have a CPT for each node in slice 1 and slice 2 so 2*numNodes
for i=1:2*nNodes
    bnet.CPD{i} = tabular_CPD(bnet, i);
    dispcpt(struct(bnet.CPD{i}).CPT);
end

engine = jtree_dbn_inf_engine(bnet);
ss = bnet.nnodes_per_slice;
T = 24;
ncases = 30;
cases = cell(1, ncases);
for i=1:ncases
   ev = sample_dbn(bnet, 'length', T);
   cases{i} = cell(ss, T);
   cases{i}(onodes, :) = ev(onodes, :);
end
long_seq = cat(2, cases{:});
[bnet2, LLtrace] = learn_params_dbn_em(engine, {long_seq}, 'max_iter', 4);
