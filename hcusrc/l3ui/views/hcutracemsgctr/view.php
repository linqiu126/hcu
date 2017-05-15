<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemsgctr */

$this->title = $model->msgid;
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemsgctrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutracemsgctr-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->msgid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->msgid], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
            'msgid',
            'msgname',
            'msgctrflag',
            'msgallow',
            'msgrestrict',
        ],
    ]) ?>

</div>
