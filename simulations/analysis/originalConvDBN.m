clear 

%Hidden variables
h_states = {'MITM', 'SRM', 'UC', 'UPS'};
%Observable variables
obs = {'MC', 'CC', 'IMD'};
%Array containing each node name 
names=[h_states, obs];

%Number of nodes 
n=length(names);

%Intraslice edges
intrac = {'MITM', 'SRM';
'MITM', 'UC';
'MITM', 'IMD';
'SRM', 'UPS';
'SRM', 'MC';
'UC', 'UPS';
'UC', 'CC'};

%Making intraslice adjiacent matrix
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

% Number of states (ns(i)=x means variable i has x states)
ns = [4 4 4 2 4 4 2];
% Reorder number of states according to topological order computed in perm
ns=ns(perm);

% Creating the DBN
bnet = mk_dbn(intra, inter, ns, 'names', names);

% Creating the CPDs

%%%%%%%%% ------- slice 1 -------

%node MITM(id=MITM) slice 1 
%parent order:{}
cpt(:,:)=[1.0, 0.0, 0.0, 0.0];
bnet.CPD{bnet.names('MITM')}=tabular_CPD(bnet,bnet.names('MITM'),'CPT',cpt);
clear cpt;

%node SpoofRepMsg(id=SRM) slice 1 
%parent order:{MITM}
cpt(1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(4,:)=[0.25, 0.25, 0.25, 0.25];
bnet.CPD{bnet.names('SRM')}=tabular_CPD(bnet,bnet.names('SRM'),'CPT',cpt);
clear cpt;

%node UnauthCmd(id=UC) slice 1 
%parent order:{MITM}
cpt(1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(4,:)=[0.25, 0.25, 0.25, 0.25];
bnet.CPD{bnet.names('UC')}=tabular_CPD(bnet,bnet.names('UC'),'CPT',cpt);
clear cpt;

%node UnstablePS(id=UPS) slice 1 
%parent order:{SRM, UC}
cpt(1,1,:)=[1.0, 0.0];
cpt(1,2,:)=[0.2058568221778793, 0.7941431778221208];
cpt(1,3,:)=[0.2058568221778793, 0.7941431778221208];
cpt(1,4,:)=[0.2058568221778793, 0.7941431778221208];
cpt(2,1,:)=[0.2058568221778793, 0.7941431778221208];
cpt(2,2,:)=[0.2058568221778793, 0.7941431778221208];
cpt(2,3,:)=[0.2058568221778793, 0.7941431778221208];
cpt(2,4,:)=[0.2058568221778793, 0.7941431778221208];
cpt(3,1,:)=[0.2058568221778793, 0.7941431778221208];
cpt(3,2,:)=[0.2058568221778793, 0.7941431778221208];
cpt(3,3,:)=[0.2058568221778793, 0.7941431778221208];
cpt(3,4,:)=[0.2058568221778793, 0.7941431778221208];
cpt(4,1,:)=[0.2058568221778793, 0.7941431778221208];
cpt(4,2,:)=[0.2058568221778793, 0.7941431778221208];
cpt(4,3,:)=[0.2058568221778793, 0.7941431778221208];
cpt(4,4,:)=[0.2058568221778793, 0.7941431778221208];
cpt1=mk_named_CPT({'SRM', 'UC', 'UPS'},names, bnet.dag, cpt);
bnet.CPD{bnet.names('UPS')}=tabular_CPD(bnet,bnet.names('UPS'),'CPT',cpt1);
clear cpt;clear cpt1;

%node MeasureCoherence(id=MC) slice 1 
%parent order:{SRM}
cpt(1,:)=[0.9772727272727273, 0.02272727272727273, 0.0, 0.0];
cpt(2,:)=[0.1325301204819277, 0.8674698795180723, 0.0, 0.0];
cpt(3,:)=[0.0, 0.2947019867549669, 0.7052980132450332, 0.0];
cpt(4,:)=[0.0, 0.08465608465608465, 0.9153439153439153, 0.0];
bnet.CPD{bnet.names('MC')}=tabular_CPD(bnet,bnet.names('MC'),'CPT',cpt);
clear cpt;

%node CommandCoherence(id=CC) slice 1 
%parent order:{UC}
cpt(1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(2,:)=[0.848901098901099, 0.1510989010989011, 0.0, 0.0];
cpt(3,:)=[0.0, 1.0, 0.0, 0.0];
cpt(4,:)=[0.25, 0.25, 0.25, 0.25];
bnet.CPD{bnet.names('CC')}=tabular_CPD(bnet,bnet.names('CC'),'CPT',cpt);
clear cpt;

%node IrregularMessageDelivery(id=IMD) slice 1 
%parent order:{MITM}
cpt(1,:)=[1.0, 0.0];
cpt(2,:)=[1.0, 0.0];
cpt(3,:)=[0.7157894736842105, 0.2842105263157895];
cpt(4,:)=[0.4433962264150944, 0.5566037735849056];
bnet.CPD{bnet.names('IMD')}=tabular_CPD(bnet,bnet.names('IMD'),'CPT',cpt);
clear cpt;

%%%%%%%%% ------- slice 2 --------

%node MITM(id=MITM) slice 2 
%parent order:{MITM}
cpt(1,:)=[0.3333333333333334, 0.6444444444444445, 0.02222222222222222, 0.0];
cpt(2,:)=[0.0, 0.6375000000000001, 0.35, 0.0125];
cpt(3,:)=[0.0, 0.0, 0.6947368421052631, 0.3052631578947368];
cpt(4,:)=[0.0, 0.0, 0.0, 1.0];
bnet.CPD{bnet.eclass2(bnet.names('MITM'))}=tabular_CPD(bnet,n+bnet.names('MITM'),'CPT',cpt);
clear cpt; 

%node SpoofRepMsg(id=SRM) slice 2 
%parent order:{MITM, SRM}
cpt(1,1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(1,2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(1,3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(1,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(2,2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt(3,1,:)=[0.7076923076923077, 0.2307692307692308, 0.04615384615384616, 0.01538461538461539];
cpt(3,2,:)=[0.0, 0.9166666666666666, 0.08333333333333333, 0.0];
cpt(3,3,:)=[0.0, 0.0, 1.0, 0.0];
cpt(3,4,:)=[0.0, 0.0, 0.0, 1.0];
cpt(4,1,:)=[0.3125, 0.375, 0.3125, 0.0];
cpt(4,2,:)=[0.0, 0.6779661016949152, 0.2711864406779661, 0.05084745762711865];
cpt(4,3,:)=[0.0, 0.0, 0.9442508710801394, 0.05574912891986063];
cpt(4,4,:)=[0.0, 0.0, 0.0, 1.0];
cpt1=mk_named_CPT_inter({'MITM', 'SRM', 'SRM'},names, bnet.dag, cpt,[1]);
bnet.CPD{bnet.eclass2(bnet.names('SRM'))}=tabular_CPD(bnet,n+bnet.names('SRM'),'CPT',cpt1);
clear cpt; clear cpt1;

%node UnauthCmd(id=UC) slice 2 
%parent order:{MITM, UC}
cpt(1,1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(1,2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(1,3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(1,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,1,:)=[1.0, 0.0, 0.0, 0.0];
cpt(2,2,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,3,:)=[0.25, 0.25, 0.25, 0.25];
cpt(2,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt(3,1,:)=[0.9560439560439561, 0.04395604395604396, 0.0, 0.0];
cpt(3,2,:)=[0.0, 1.0, 0.0, 0.0];
cpt(3,3,:)=[6.103515625E-5, 6.103515625E-5, 0.99981689453125, 6.103515625E-5];
cpt(3,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt(4,1,:)=[0.7636363636363636, 0.2181818181818182, 0.01818181818181818, 0.0];
cpt(4,2,:)=[0.0, 0.9822485207100592, 0.01775147928994083, 0.0];
cpt(4,3,:)=[0.0, 0.0, 1.0, 0.0];
cpt(4,4,:)=[0.25, 0.25, 0.25, 0.25];
cpt1=mk_named_CPT_inter({'MITM', 'UC', 'UC'},names, bnet.dag, cpt,[1]);
bnet.CPD{bnet.eclass2(bnet.names('UC'))}=tabular_CPD(bnet,n+bnet.names('UC'),'CPT',cpt1);
clear cpt; clear cpt1;

%node UnstablePS(id=UPS) slice 2 
%parent order:{SRM, UC, UPS}
cpt(1,1,1,:)=[0.9855072463768116, 0.01449275362318841];
cpt(1,1,2,:)=[0.0, 1.0];
cpt(1,2,1,:)=[1.0, 0.0];
cpt(1,2,2,:)=[0.5, 0.4999999999999999];
cpt(1,3,1,:)=[0.4636093292171563, 0.5363906707828437];
cpt(1,3,2,:)=[0.5000000000000001, 0.5];
cpt(1,4,1,:)=[0.4636093292171563, 0.5363906707828437];
cpt(1,4,2,:)=[0.5, 0.5];
cpt(2,1,1,:)=[0.9827586206896551, 0.01724137931034483];
cpt(2,1,2,:)=[0.0, 1.0];
cpt(2,2,1,:)=[0.9375, 0.0625];
cpt(2,2,2,:)=[0.0, 1.0];
cpt(2,3,1,:)=[1.0, 0.0];
cpt(2,3,2,:)=[0.5000000000000003, 0.4999999999999998];
cpt(2,4,1,:)=[0.4636093292171563, 0.5363906707828437];
cpt(2,4,2,:)=[0.5, 0.5];
cpt(3,1,1,:)=[0.9607843137254902, 0.0392156862745098];
cpt(3,1,2,:)=[0.0, 1.0];
cpt(3,2,1,:)=[0.8983050847457628, 0.1016949152542373];
cpt(3,2,2,:)=[0.0, 1.0];
cpt(3,3,1,:)=[0.8260869565217391, 0.1739130434782609];
cpt(3,3,2,:)=[0.0, 1.0];
cpt(3,4,1,:)=[0.4636093292171563, 0.5363906707828437];
cpt(3,4,2,:)=[0.5, 0.5];
cpt(4,1,1,:)=[1.0, 0.0];
cpt(4,1,2,:)=[0.0, 1.0];
cpt(4,2,1,:)=[0.8387096774193548, 0.1612903225806452];
cpt(4,2,2,:)=[0.0, 1.0];
cpt(4,3,1,:)=[0.9473684210526315, 0.05263157894736842];
cpt(4,3,2,:)=[0.0, 1.0];
cpt(4,4,1,:)=[0.4636093292171563, 0.5363906707828437];
cpt(4,4,2,:)=[0.5, 0.5];
cpt1=mk_named_CPT_inter({'SRM', 'UC', 'UPS', 'UPS'},names, bnet.dag, cpt,[1,2]);
bnet.CPD{bnet.eclass2(bnet.names('UPS'))}=tabular_CPD(bnet,n+bnet.names('UPS'),'CPT',cpt1);
clear cpt; clear cpt1;

% choose the inference engine
ec='JT';

% ff=0 --> no fully factorized  OR ff=1 --> fully factorized
ff=0;

% list of clusters
if (ec=='JT')
	engine=bk_inf_engine(bnet, 'clusters', 'exact'); %exact inference
else
	if (ff==1)
		engine=bk_inf_engine(bnet, 'clusters', 'ff'); % fully factorized
	else
		clusters={[]};
		engine=bk_inf_engine(bnet, 'clusters', clusters);
	end
end

% IMPORTANT: GeNIe start slices from 0,
T=25; %max time span thus from 0 to T-1
tStep=1; %Time Step
evidence=cell(n,T); % create the evidence cell array

% Evidence
% first cells of evidence are for time 0
% Inference algorithm (filtering / smoothing)
filtering=0;
% filtering=0 --> smoothing (is the default - enter_evidence(engine,evidence))
% filtering=1 --> filtering
if ~filtering
	fprintf('\n*****  SMOOTHING *****\n\n');
else
	fprintf('\n*****  FILTERING *****\n\n');
end

[engine, loglik] = enter_evidence(engine, evidence, 'filter', filtering);

% analysis time is t for anterior nodes and t+1 for ulterior nodes
for t=1:tStep:T-1
%t = analysis time

% create the vector of marginals
% marg(i).T is the posterior distribution of node T
% with marg(i).T(false) and marg(i).T(true)

% NB. if filtering then ulterior nodes cannot be marginalized at time t=1

if ~filtering
	for i=1:(n*2)
		marg(i)=marginal_nodes(engine, i , t);
	end
else
	if t==1
		for i=1:n
			marg(i)=marginal_nodes(engine, i, t);
		end
	else
		for i=1:(n*2)
			marg(i)=marginal_nodes(engine, i, t);
		end
	end
end

% Printing results
% IMPORTANT: To be consistent with GeNIe we start counting/printing time slices from 0


% Anterior nodes are printed from t=1 to T-1
fprintf('\n\n**** Time %i *****\n****\n\n',t-1);
%fprintf('*** Anterior nodes \n');
for i=1:n
	if isempty(evidence{i,t})
		for k=1:ns(i)
			fprintf('Posterior of node %i:%s value %i : %d\n',i, names{i}, k, marg(i).T(k));
		end
			fprintf('**\n');
		else
			fprintf('Node %i:%s observed at value: %i\n**\n',i,names{i}, evidence{i,t});
		end
	end
end

% Ulterior nodes are printed at last time slice
fprintf('\n\n**** Time %i *****\n****\n\n',T-1);
%fprintf('*** Ulterior nodes \n');
for i=(n+1):(n*2)
	if isempty(evidence{i-n,T})
		for k=1:ns(i-n)
			fprintf('Posterior of node %i:%s value %i : %d\n',i, names{i-n}, k, marg(i).T(k));
		end
		fprintf('**\n');
	else
		fprintf('Node %i:%s observed at value: %i\n**\n',i,names{i-n}, evidence{i-n,T});
	end
end