vret=[];
exit=0;
compile;
time=1;
outputs={'gE' };
%initial call
[V, out1] = conductance('dummy',outputs);
global k
k=0;
% Membrane potential
figure(1);
set(gcf,'keypress','global k;k=1;disp(k)');
hV = imagesc(V.Vex ,[-80 -55]);
gridsize=length(V.Vex(:,1));
set(gca,'XLim',[0.5 gridsize+0.5],'YLim',[0.5 gridsize+0.5],'XTick',[],'YTick',[],...
'XTickLabel',[],'YTickLabel',[],'XGrid','off','YGrid','off');
colormap('default');
colorbar;
hT = title('Time: 1');
figure(2);
set(gcf,'keypress','global k;k=1;disp(k)');
hVi = imagesc(V.Vin ,[-80 -55]);
gridsize=length(V.Vin(:,1));
set(gca,'XLim',[0.5 gridsize+0.5],'YLim',[0.5 gridsize+0.5],'XTick',[],'YTick',[],...
'XTickLabel',[],'YTickLabel',[],'XGrid','off','YGrid','off');
colormap('default');
title('Vin');
colorbar;
h=[];
for i=1:length(out1)
    h(i)=setupplot(out1{i},i+2,outputs);
end

while time<8050
    time=time+1;
    [V, out1] =conductance(V,outputs);
    if (time > 1000)
     %   vret(time-1000,:,:)=out1{1}.data;
         vret(time-1000,:,:)=V.Vex;
    end
    if (mod(time,10)==0)
        set(hV,'CData',V.Vex);
        set(hVi,'CData',V.Vin);
        set(hT,'String',sprintf('Time: %.1f',time));
        for i=1:length(out1)
            set(h(i),'CData',out1{i}.data);  
        end
        drawnow;
        if (k==1)
            return
        end
    end
end
