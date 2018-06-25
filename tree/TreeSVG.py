#coding:utf8
import math
import sys
sys.path.append('../')

import tree_util 
from tree_node import TreeNode

gPageHead = '''<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
xmlns:xlink="http://www.w3.org/1999/xlink"
xmlns:a3="http://ns.adobe.com/AdobeSVGViewerExtensions/3.0/"
a3:scriptImplementation="Adobe"
onload ="getSVGDoc(evt)"\n''' 

gPageFunction = '''<script type="text/javascript">
<![CDATA[
var svgdoc, svgroot, infoboxElem, infoboxTextElem; 

function getSVGDoc(load_evt)
{
    svgdoc = load_evt.target.ownerDocument;
    svgroot = svgdoc.documentElement;
    infoboxElem = svgdoc.getElementById("infobox");
    infoboxTextElem = svgdoc.getElementById("infobox_text");
}

function ShowTooltip(evt)
{
    var posx, posy, curtrans, ctx, cty;
    posx = evt.clientX;
    posy = evt.clientY;
    curtrans = svgroot.currentTranslate;
    ctx = curtrans.x;
    cty = curtrans.y;

    var targetshape = evt.target;
    if (targetshape.getAttribute("class") != "info") {
        return;
    }
    offPosX = Math.round(svgroot.scrollLeft + posx - ctx);
    offPosY = Math.round(svgroot.scrollTop  + posy - cty); 

    var ratio = window.innerWidth / window.outerWidth;
    var font_size = 15 * ratio * 2;
    console.log(ratio, font_size);

    svgWidth = parseFloat(svgroot.getAttribute('width'));
    svgHeight = parseFloat(svgroot.getAttribute('height'));


    infoboxTextElem.setAttribute("width", 100 * ratio); 
    infoboxTextElem.setAttribute("style", "fill: #0000CC; font-size: " + font_size + "px; visibility: visible");

    // infoboxTextElem.innerHTML = "(" + posx + "," + posy + ")(" + offPosX + "," + offPosY + ")";
    // infoboxTextElem.childNodes.item(0).data = "(" + posx + "," + posy + ")(" + offPosX + "," + offPosY + ")";
    // infoboxTextElem.childNodes.item(1).data = "bbb";

    // infoboxTextElem.childNodes.item(0).data = "(" + posx + "," + posy + ")(" + offPosX + "," + offPosY + ")";

    // infoboxElem.setAttribute("width", infoboxTextElem.getComputedTextLength() + font_size * 2); 
    infoboxElem.setAttribute("width", font_size * 12); 
    infoboxElem.setAttribute("height", font_size * 3); 
    infoboxElem.setAttribute("style", "fill: #FFFFCC; stroke: #000000; stroke-width: 0.5px; visibility: visible");

    if ((offPosX + parseFloat(infoboxElem.getAttribute('width'))) > svgWidth) {
        offPosX -= infoboxElem.getAttribute('width') + font_size;
    }
    infoboxElem.setAttribute("x", offPosX);
    infoboxElem.setAttribute("y", offPosY);
    infoboxTextElem.setAttribute("x", offPosX + font_size);
    // infoboxTextElem.setAttribute("y", offPosY + font_size * 1.25); 
    infoboxTextElem.setAttribute("y", offPosY + font_size); 

    span_x = infoboxTextElem.getAttribute("x");
    tspan_size = (infoboxTextElem.childNodes.length - 1) / 2;
    node_info_list = targetshape.getAttribute("nodeinfo").split('*');
    var maxTextSize = 0;
    for (var i = 0; i < tspan_size; ++i) {
        curIdx = 2 * i + 1;
        infoboxTextElem.childNodes.item(curIdx).setAttribute("x", span_x);
        infoboxTextElem.childNodes.item(curIdx).setAttribute("dy", font_size);
        infoboxTextElem.childNodes.item(curIdx).innerHTML = node_info_list[i];
        if (node_info_list[i].length > maxTextSize)
            maxTextSize = node_info_list[i].length;
    }

    // infoboxElem.setAttribute("height", font_size * 3); 
    infoboxElem.setAttribute("width", font_size * maxTextSize * 0.75); 
    infoboxElem.setAttribute("height", font_size * tspan_size * 1.5); 

    console.log("infoboxTextElem:",
                maxTextSize, 
                span_x,
                tspan_size,
                infoboxTextElem.childNodes.length,
                infoboxTextElem.childNodes.item(0), 
                infoboxTextElem.childNodes.item(1).innerHTML,
                infoboxTextElem.childNodes.item(1),
                infoboxTextElem.childNodes.item(2),
                infoboxTextElem.childNodes.item(3),
                infoboxTextElem.childNodes.item(4),
                infoboxTextElem.innerHTML
                )



    console.log(new Date(), "click", posx, posy, offPosX, offPosY, svgWidth);
}

function HideTooltip(evt)
{
    infoboxElem.setAttribute("style", "visibility: hidden");
    infoboxTextElem.setAttribute("style", "visibility: hidden");
    // console.log(new Date(), "hidden");
}

function ZoomControl()
{
    var curzoom;
    curzoom = svgroot.currentScale;
    svgdoc.getElementById("tooltip").setAttribute("transform","scale("+1/curzoom+")");
}

]]> \n\
</script>\n''' 

gPageTail_1 = '''<g id="tooltip" style="pointer-events: none">
    <rect id="infobox" x="0" y="0" rx="5" ry="5" width="100" height="200"
        style="visibility: hidden"/>
    <text id="infobox_text" x="0" y="0" style="visibility: hidden">
'''

gPageTail_2 = '''</text>
</g>'''





def addHeadInfo(pageWidth, pageHeight):
    global gPageHead
    global gPageFunction
    return '%s width="%d" height="%d"\nversion="1.1">\n%s' \
                % (gPageHead, pageWidth, pageHeight, gPageFunction)
 
class TreeSVG(object):
    def __init__(self, root):
        self.root = root
        self.gDepth = tree_util.treeDepth(root)
        # self.gRadius = 30
        self.gRadius = 35
        self.gFrontSize = 10
        
        self.gOffSetX = 30
        self.width = self.gOffSetX * (2 ** (self.gDepth - 1)) * 2 * 2
        self.offSetY = self.gOffSetX * 2

        self.pageWidth = self.width + 2 * self.gOffSetX
        self.pageHeight = self.pageWidth

        self.treeNodeInfoSize = 5

        self.pageHtml = addHeadInfo(self.pageWidth, self.pageHeight)



    def drawLine(self, fromXY, toXY):
        global pageHtml
        attrDict = {}
        attrDict['x1'] = fromXY[0]
        attrDict['y1'] = fromXY[1]
        attrDict['x2'] = toXY[0]
        attrDict['y2'] = toXY[1]
        attrDict['style'] = 'stroke:rgb(0,0,0);stroke-width:2'
        attrStr = ' '.join(map(lambda it : it[0] + '=' + '"' + str(it[1]) + '"', attrDict.items()))
        # pageHtml += '<line ' + attrStr + '/>' 
        self.pageHtml += '<line ' + attrStr + '/>' 

    def drawDecisionNode(self, XY, nodeTextList):
        global pageHtml
        attrDict = {}
        attrDict['nodeinfo'] = '*'.join(nodeTextList)
        attrDict['cx'] = XY[0]
        attrDict['cy'] = XY[1]
        attrDict['r'] = self.gRadius
        attrDict['onclick'] = 'ShowTooltip(evt)'
        attrDict['class'] = 'info'
        attrDict['fill-opacity'] = '0.0'
        attrDict['stroke-opacity'] = '1'
        attrDict['onmouseout'] = 'HideTooltip(evt)'
        attrDict['style'] = 'stroke:black;stroke-width:2; fill:white; fill-opacity:0.9'
        attrStr = ' '.join(map(lambda it : it[0] + '=' + '"' + str(it[1]) + '"', attrDict.items()))
        # pageHtml += '<circle ' + attrStr + '/>' 
        self.pageHtml += '\n<circle ' + attrStr + '/>' 

    def drawText(self, XY, textList):
        self.pageHtml += '<g id="testG" onclick="ShowTooltip(evt)" onmouseout = "HideTooltip(evt)">'
        tAttrDict = {}
        tAttrDict['nodeinfo'] = '*'.join(textList)
        tAttrDict['class'] = 'target'
        tAttrDict['x'] = XY[0]
        tAttrDict['y'] = XY[1]
        tAttrDict['font-family'] = 'Verdana'
        tAttrDict['font-size'] = self.gFrontSize
        tAttrDict['fill'] = 'black'
        tAttrStr = ' '.join(map(lambda it : it[0] + '=' + '"' + str(it[1]) + '"', tAttrDict.items()))

        self.pageHtml += '<text ' + tAttrStr + '>'

        tsAttrDict = {}
        tsTextLen = max(map(lambda x : len(x), textList))
        tsAttrDict['x'] = XY[0] - tsTextLen * self.gFrontSize / 2 / 2
        # tsAttrDict['textLength'] = tsTextLen
        # print tsAttrDict['textLength']
        tsAttrDict['lengthAdjust'] = 'spacing'
        tsAttrStr = ' '.join(map(lambda it : '%s="%s"' % (it[0], str(it[1])), \
                                 tsAttrDict.items()))

        for i, textItem in enumerate(textList):
            textLength = 'textLenght="%d"' % (len(textList[i]) * self.gFrontSize)
            if i == 0:
                tsAttrY = 'y="%d"' % (int(XY[1] - self.gFrontSize * 0.5 * len(textList) * 0.5))
                self.pageHtml += '\n<tspan %s %s %s> %s </tspan>' % \
                        (tsAttrStr, tsAttrY, textLength, textList[i])
            else:
                self.pageHtml += '\n<tspan %s %s dy="%s"> %s </tspan>' % \
                        (tsAttrStr, textLength, str(self.gFrontSize), textList[i])

        self.pageHtml += '\n</text>'
        self.pageHtml += '</g>'
        self.pageHtml += ''

    def drawNode(self, XY, nodeTextList, curOffsetX, curOffsetY, \
                 leftFlag, rightFlag):
        self.drawDecisionNode(XY, nodeTextList)
        self.pageHtml += '\n'
        self.drawText((XY[0], XY[1] ), nodeTextList)
        # self.drawText((XY[0] - self.gRadius / 2, XY[1] ), [nodeText, ''])
        # self.drawText((XY[0], XY[1] ), [nodeText, 'ab', '我是'])


        if leftFlag is not None:
            self.pageHtml += '\n'
            self.drawLine((XY[0], XY[1] + self.gRadius), \
                    (XY[0] - curOffsetX, XY[1] + self.gRadius + curOffsetY))

        if rightFlag is not None:
            self.pageHtml += '\n'
            self.drawLine((XY[0], XY[1] + self.gRadius), \
                    (XY[0] + curOffsetX, XY[1] + self.gRadius + curOffsetY))

    def drawTree(self, parent, treeRoot, level):
        depth = tree_util.treeDepth(treeRoot)
        if depth is 0:
            return None

        childrenNum = 2 ** (depth - 1)

        nodeTextList = ['level:%d' % (level), \
                        'fea_idx:%d' % (treeRoot.idx), \
                        'split:%f' % (treeRoot.split), \
                        'size:%d' % (treeRoot.size), \
                        'value:%.3f' % (treeRoot.value)
                        ]
        self.drawNode(parent, \
                      # str(self.gDepth - depth), \
                      nodeTextList, \
                      childrenNum * self.gOffSetX * 2 / 2, \
                      math.sqrt(depth) * self.gOffSetX, \
                      treeRoot.left, treeRoot.right)

        toX = parent[0] - childrenNum * self.gOffSetX * 2 / 2
        toY = parent[1] + self.gRadius + math.sqrt(depth) * self.gOffSetX
        self.drawTree((toX, toY), treeRoot.left, level + 1)

        toX = parent[0] + childrenNum * self.gOffSetX * 2 / 2
        toY = parent[1] + self.gRadius + math.sqrt(depth) * self.gOffSetX
        self.drawTree((toX, toY), treeRoot.right, level + 1)

    def end(self):
        global gPageTail_1
        global gPageTail_2
        self.pageHtml += gPageTail_1
        tspanStr = ''
        for i in range(self.treeNodeInfoSize):
            tspanStr += '<tspan> </tspan>\n'
        self.pageHtml += '\n' + tspanStr
        self.pageHtml += gPageTail_2
        self.pageHtml += '</svg>'

    def draw(self):
        self.drawTree((self.width / 2, self.gRadius + 10), self.root, 1)
        self.end()

'''
pain.drawTree((width / 2, gRadius * 2), gDepth)
pain.end()
'''
if __name__ == '__main__':

    n1 = TreeNode()
    n2 = TreeNode()
    n3 = TreeNode()
    n4 = TreeNode()

    n1.left = n2
    n1.right = n3

    n2.right = n4

    pain = TreeSVG(n1)
    pain.draw()

    outSvg = open('test.svg', 'w') 
    print >> outSvg, pain.pageHtml

