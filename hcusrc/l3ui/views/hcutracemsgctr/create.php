<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemsgctr */

$this->title = 'Create Hcutracemsgctr';
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemsgctrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutracemsgctr-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
