import cv2


cards = ['C','D','H','S']
for i in range(52):
    file_name = '/%d%c.png'%(i%13+1,cards[i//13])
    print(file_name)
    img = cv2.imread('card_origin'+file_name)
    
    img = cv2.resize(img,None,fx=0.3,fy=0.3)
    cv2.imshow(file_name,img)
    # cv2.waitKey(1)
    cv2.imwrite('card'+file_name,img)

# cv2.destroyAllWindows()
