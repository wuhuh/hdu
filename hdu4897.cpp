#include<stdio.h>
#include<vector>
using namespace std;
#define maxx 100050
vector<int> line[maxx];
int d[maxx],fa[maxx],son[maxx],size[maxx],top[maxx],num[maxx],rnum[maxx];
int root[4*maxx];
int lazy[4*maxx];//标记为1时表示该区间都要翻转
int laa[4*maxx];//标记为1时表示能到该区间上点的轻链都需要翻转
int cnt;
int n;
//先剖分
void dfs1(int u,int v){
    d[v]=d[u]+1;
    fa[v]=u;
    son[v]=0;
    size[v]=1;
    for(int i=0;i<line[v].size();i++){
        int to=line[v][i];
        if(to!=u) {
            dfs1(v,to);
            size[v]+=size[to];
            if(size[to]>size[son[v]]) son[v]=to;
        }
    }
}
void dfs2(int u,int topp){
    top[u]=topp;
    num[u]=++cnt;
    rnum[cnt]=u;
    if(son[u]){
        dfs2(son[u],topp);
        for(int i=0;i<line[u].size();i++){
            int to=line[u][i];
            if(to!=son[u]&&to!=fa[u]) dfs2(to,to);
        }
    }
}
//再建树，用dfs序建树，同一个重链的序号相邻
void pushdown(int now,int len){
    if(lazy[now]){
        lazy[now*2]^=1;
        lazy[now*2+1]^=1;
        root[now*2]=len-(len)/2-root[now*2];
        root[now*2+1]=(len)/2-root[now*2+1];
        lazy[now]=0;
    }
    if(laa[now]){
        laa[now*2]^=1;
        laa[now*2+1]^=1;
        laa[now]=0;
    }
}
void pushup(int now){
    root[now]=root[now*2]+root[now*2+1];
}
void bulidtree(int l,int r,int now){//新建空树，顺便清空标记值
    laa[now]=lazy[now]=0;
    if(l==r){
        root[now]=0;
        return;
    }
    int mid=(l+r)/2;
    bulidtree(l,mid,now*2);
    bulidtree(mid+1,r,now*2+1);
    pushup(now);
}
void changetree(int l,int r,int L,int R,int now){//将[l,r]区间内边翻转
    if(L>r||R<l) return;
    int mid=(L+R)/2;
    if(L>=l&&R<=r) {
        root[now]=(R-L+1)-root[now];
        lazy[now]^=1;
        return;
    }
    pushdown(now,R-L+1);
    changetree(l,r,L,mid,now*2);
    changetree(l,r,mid+1,R,now*2+1);
    pushup(now);
}
void addlaa(int l,int r,int L,int R,int now){//给[l,r]区间内点更新laa标记
    if(L>r||R<l) return;
    int mid=(L+R)/2;
    if(L>=l&&R<=r){
        laa[now]^=1;
        return;
    }
    pushdown(now,R-L+1);
    addlaa(l,r,L,mid,now*2);
    addlaa(l,r,mid+1,R,now*2+1);
}
int findlaa(int x,int L,int R,int now){//查找x的laa标记
    if(L==R) return laa[now];
    pushdown(now,R-L+1);
    int mid=(L+R)/2;
    if(x<=mid) return findlaa(x,L,mid,now*2);
    return findlaa(x,mid+1,R,now*2+1);
}
void lca_in(int x,int y){//翻转[x,y]途径边，利用重链
    while(top[x]!=top[y]){
        if(d[top[x]]>d[top[y]]){
            changetree(num[top[x]],num[x],1,n,1);//将到另一条重链前的边都翻转
            x=fa[top[x]];
        }
        else {
            changetree(num[top[y]],num[y],1,n,1);
            y=fa[top[y]];
        }
    }
    int tmp;
    if(d[x]==d[y]) return;//已经到两点最短路径的公共点
    if(d[x]>d[y]){
        tmp=x;x=y;y=tmp;
    }
    changetree(num[x]+1,num[y],1,n,1);
}
void lca_near(int x,int y){//翻转[x,y]途径临边，利用重链
    int tmp;
    while(top[x]!=top[y]){
        if(d[top[x]]<d[top[y]]){tmp=x;x=y;y=tmp;}
        addlaa(num[top[x]],num[x],1,n,1);//给途径点添加标记laa
        if(son[x]) changetree(num[x]+1,num[x]+1,1,n,1);//如果重链底部的点有重链，直接翻转，因为标记laa对重链无效
        changetree(num[top[x]],num[top[x]],1,n,1);//直接翻转重链头节点到其父节点的边。也是标记对其无效
        x=fa[top[x]];
    }
    if(d[x]>d[y]) {
        tmp=x;x=y;y=tmp;
    }
    addlaa(num[x],num[y],1,n,1);
    if(son[y]) changetree(num[y]+1,num[y]+1,1,n,1);
     changetree(num[x],num[x],1,n,1);
}
int query(int l,int r,int L,int R,int now){//区间求和
    if(L>r||R<l) return 0;
    if(L>=l&&R<=r) return root[now];
    pushdown(now,R-L+1);
    int mid=(L+R)/2;
    return query(l,r,L,mid,now*2)+query(l,r,mid+1,R,now*2+1);
}
int lca_sum(int x,int y){//lca区间求和，利用重链
    int sum=0;
    int tmp;
    while(top[x]!=top[y]){
        if(d[top[x]]<d[top[y]]){tmp=x;x=y;y=tmp;}
        if(x!=top[x]) sum+=query(num[top[x]]+1,num[x],1,n,1);//重链上不可能有操作3的翻转，直接正常求和
        sum+=(query(num[top[x]],num[top[x]],1,n,1))^(findlaa(num[fa[top[x]]],1,n,1));//top[x]所指向的边为轻链，需要考虑父节点的laa标记值
        x=fa[top[x]];
    }
    if(d[x]==d[y]) return sum;
    if(d[x]>d[y]){
        tmp=x;x=y;y=tmp;
    }
    sum+=query(num[x]+1,num[y],1,n,1);
    return sum;
}
int main(void){
    int T,x,y,z,q;
    scanf("%d",&T);
    while(T--){
        scanf("%d",&n);
        for(int i=1;i<=n;i++) line[i].clear();
        for(int i=1;i<n;i++) {
            scanf("%d%d",&x,&y);
            line[x].push_back(y);
            line[y].push_back(x);
        }
        d[0]=0;
        size[0]=0;
        cnt=0;
        dfs1(0,1);
        dfs2(1,1);
        bulidtree(1,n,1);
        scanf("%d",&q);
        while(q--){
            scanf("%d%d%d",&x,&y,&z);
            if(x==1){lca_in(y,z);}
            else if(x==2){lca_near(y,z);}
            else{printf("%d\n",lca_sum(y,z));}
        }
    }
}